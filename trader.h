#ifndef __trade__
#define __trade__
#include <iostream>
#include <string>
#include <cstdlib>
#include "solsystem.h"

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
