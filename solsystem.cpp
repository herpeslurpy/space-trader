#include <iostream>
#include <string>
#include <cstdlib>
#include "solsystem.h"
using namespace std;

/*
A star system object.
Contains a name, the prices of the three commodities,
and a bool to define whether or not it's the current system.
*/
solsystem::solsystem(string n, int food, int metal, int medical){
	name = n;
	foodPrice = food;
	metalPrice = metal;
	medicalPrice = medical;

}
solsystem::solsystem(){

}

string solsystem::getName(){
	return name;
}

int solsystem::getFoodPrice(){
	return foodPrice;
}

int solsystem::getMetalPrice(){
	return metalPrice;
}

int solsystem::getMedicalPrice(){
	return medicalPrice;
}

bool solsystem::getIsCurrent(){
	return isCurrent;
}

void solsystem::setCurrent(bool c){
	isCurrent = c;
}


