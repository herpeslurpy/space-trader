#ifndef __solsystem__
#define __solsystem__

#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

class solsystem{

private:
	string name;
	int foodPrice;
	int metalPrice;
	int medicalPrice;
	bool isCurrent;

public:
	string getName();
	int getFoodPrice();
	int getMetalPrice();
	int getMedicalPrice();
	bool getIsCurrent();
	void setCurrent(bool c);
	solsystem(string n, int food, int metal, int medical);
	solsystem();
};
#endif
