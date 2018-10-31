#include <iostream>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;
string MOVEACTIONS[] = { "a", "d", "w", "s" };
string SHOOTACTIONS[] = { "f1", "f2", "f3", "f4" };
struct Plan {

	char default_commands[4] = { 'm', 'm' ,'s', 's' }; // balanced actions
	char defensive_commands[4] = { 'm', 'm' ,'m', 's' }; //more movement
	char aggressive_commands[4] = { 's', 's' ,'s', 's' }; //more shooting
	char* current_commands = default_commands;
	char getAction() {
		return (current_commands[rand() % 4]);
	}
	void changePlan(string planType) {
		if (planType == "agressive") {
			current_commands = aggressive_commands;
		}
		if (planType == "defensive") {
			current_commands = defensive_commands;
		}
	}
};
struct Robot 
{
    int hp;
    int x1;
    int y1;
    int x2;
    int y2;
	Plan strategy;

	Robot(int hp, int x1, int y1, int x2, int y2) :
		hp(hp),
		x1(x1),
		y1(y1),
		x2(x2),
		y2(y2)
    {
    }
};

struct Shoot
{
    int x;
    int y;
    int dir;

    Shoot(int x, int y, int dir) :
        x(x),
        y(y),
        dir(dir)
    {
    }
};

vector<Shoot> shoots;

bool locationIsInRobot(const Robot& robot, const int& xPosition, const int& yPosition)
{
    return xPosition >= robot.x1 && xPosition <= robot.x2 && 
            yPosition >= robot.y1 && yPosition <= robot.y2;
}

bool isShootLocation(const int& xPosition,const int& yPosition)
{
    for (Shoot &shoot : shoots)
        if (xPosition == shoot.x && yPosition == shoot.y)
            return true;

    return false;
}

void printRobotHP(const Robot& robot)
{
    for (int hp = 0; hp < robot.hp; hp++)
        cout << "#";
    for (int hits = 0; hits < (10 - robot.hp); hits++)
        cout << " ";
}

void printGameField(Robot &robot, Robot &robot2)
{
    for (int yPosition = 0; yPosition < 20; yPosition++) {
        for (int xPosition = 0; xPosition < 20; xPosition++) {
            if (locationIsInRobot(robot,xPosition,yPosition))
                cout << "X";
            else if (locationIsInRobot(robot2,xPosition,yPosition))
                cout << "Y";
            else
                cout << (isShootLocation(xPosition, yPosition) ? "*" : "_");
        }
        cout << "|" << endl;
    }
}

void printHPArea(Robot &robot, Robot &robot2)
{
    cout << "HP BOT 1: ";
    printRobotHP(robot);
    cout << "|" << endl;
    cout << "HP BOT 2: ";
    printRobotHP(robot2);
    cout << "|" << endl;
}

void printscreen(Robot &robot, Robot &robot2)
{
#ifdef _WIN32
	system("CLS"); //clears screen for Windows OS
#else
	system("clear"); //clears screen for UNIX OS
#endif
    printGameField(robot, robot2);
    printHPArea(robot, robot2);
}
void robot_action(string acts[], Robot& r) {
	string c = acts[rand() % 4];
	if (c == "f1") { // esq
		shoots.push_back(Shoot(r.x1 - 1, r.y1, 0));
	}
	else if (c == "f2") { // dir
		shoots.push_back(Shoot(r.x2 + 1, r.y1, 1));
	}
	else if (c == "f3") { // cima
		shoots.push_back(Shoot(r.x1, r.y1 - 1, 2));
	}
	else if (c == "f4") { // baixo
		shoots.push_back(Shoot(r.x1, r.y2 + 1, 3));
	}
}
void robot_movements(string acts[], Robot& r)
{
    string c = acts[rand() % 4];
    if (c == "a") {
        if (r.x1 > 0) {
            r.x1--;
            r.x2--;
        }
    }
    else if (c == "d") {
        if (r.x2 < 19) {
            r.x1++;
            r.x2++;
        }
    }
    else if (c == "w") {
        if (r.y1 > 0) {
            r.y1--;
            r.y2--;
        }
    }
    else if (c == "s") {
        if (r.y2 < 19) {
            r.y1++;
            r.y2++;
        }
    }


}
void RobotActions(Robot& r) {
	if (r.strategy.getAction() == 'm') {
		robot_movements(MOVEACTIONS, r);

	}
	else {
		robot_action(SHOOTACTIONS, r);
	}
	//ChangePlan
	if (r.hp < 5) {
		r.strategy.changePlan("aggressive");
	}
}

int main()
{
    srand(time(0));
    Robot robot(10, 0, 0, 1, 1);
    Robot robot2(10, 18, 18, 19, 19);

	string plans[] = { "aggressive" , "defensive"};
	//randomly choose an AI type - can be user set in future.
	robot.strategy.changePlan(plans[rand() % 2]); 
	robot2.strategy.changePlan(plans[rand() % 2]);


    while (true) {
        this_thread::sleep_for(chrono::microseconds(50000));

        for (auto it = shoots.begin(); it != shoots.end(); ) {
            if ((robot.x1 <= (*it).x && (*it).x <= robot.x2) && (robot.y1 <= (*it).y && (*it).y <= robot.y2)) {
                robot.hp--;
                it = shoots.erase(it);
                continue;
            }

            if ((robot2.x1 <= (*it).x && (*it).x <= robot2.x2) && (robot2.y1 <= (*it).y && (*it).y <= robot2.y2)) {
                robot2.hp--;
                it = shoots.erase(it);
                continue;
            }

            ++it;
        }

        if (robot.hp == 0) {
            cout << "ROBOT 2 WINS!!!" << endl;
            break;
        }

        if (robot2.hp == 0) {
            cout << "ROBOT 1 WINS!!!" << endl;
            break;
        }

        printscreen(robot, robot2);
		//Robot 1
	 RobotActions(robot);
	 RobotActions(robot2);

        for (Shoot &s : shoots) {
            if (s.dir == 0) {
                if (s.x > 0)
                    s.x--;
                else {
                    s.x = -1;
                    s.y = -1;
                }
            }
            else if (s.dir == 1) {
                if (s.x < 19)
                    s.x++;
                else {
                    s.x = -1;
                    s.y = -1;
                }
            }
            else if (s.dir == 2) {
                if (s.y > 0)
                    s.y--;
                else {
                    s.x = -1;
                    s.y = -1;
                }
            }
            else if (s.dir == 3) {
                if (s.y < 19)
                    s.y++;
                else {
                    s.x = -1;
                    s.y = -1;
                }
            }
        }

        vector<Shoot> tmp;
        for (Shoot &s : shoots)
            if (s.x != -1 && s.y != -1)
                tmp.push_back(s);
        shoots.clear();
        for (Shoot &s : tmp)
            shoots.push_back(s);
    }
    
    return 0;
}
