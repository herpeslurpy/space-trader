#ifndef __trade__
#define __trade__
#include <iostream>
#include <string>
#include <cstdlib>
#include "solsystem.h"
using namespace std;

//Player info
string playerName = "Ian";
int creditsHeld = 100;

//Ship info
string shipName = "Snaggletooth";
int cargoCap = 50;
int spaceLeft = 50;
int foodHeld = 0;
int metalHeld = 0;
int medicalHeld = 0;
double attackChance = ((double)foodHeld + (double)metalHeld + (double)medicalHeld)/((double)cargoCap*8);

int main(int argc, char **argv);
void showPlayerData();
void showShipData();
void showSystemData(solsystem *newS);
void openJumpDialog();
bool systemExists(string n);
void updateSystemList();
void openManual();
void openMarket(solsystem *tmp);
void loadSave();
void saveData();
#endif
