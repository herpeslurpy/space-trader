/*
Patrick Letts, 2016
Distributed under the MIT license.
Copy available in ./LICENSE
*/

#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <curses.h>
#include "solsystem.h"
using namespace std;

//Player info
string playerName;;
int creditsHeld = 100;

//Ship info
string shipName;;
int cargoCap = 50;
int spaceLeft = 50;
int foodHeld = 0;
int metalHeld = 0;
int medicalHeld = 0;
double attackChance = ((double)foodHeld + (double)metalHeld + (double)medicalHeld)/((double)cargoCap*8);

//Systems
solsystem *Alpha = new solsystem("Alpha", 10, 15, 20);
solsystem *Beta = new solsystem("Beta", 19, 5, 12);
solsystem *Delta = new solsystem("Delta", 30, 4, 10);
solsystem *Gamma = new solsystem("Gamma", 24, 14, 6);
solsystem *current = Beta;
solsystem **list = new solsystem*[5];
int numSystems = 4;

/*
When creating a new window object, we also want a default border around it
*/
WINDOW *create_newwin(int height, int width, int starty, int startx){
	WINDOW *local_win;
	
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
	wborder(local_win, '|', '|', '-', '-', 'X', 'X', 'X', 'X');
	
	wrefresh(local_win);
	return local_win;
}

/*
Before actually deleting a window, make its border entirely transparent
*/
void destroy_win(WINDOW *local_win){
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
}

/*
List basic player information from global vars in main.h;
also update the chance of being attacked by pirates
*/
void showPlayerData(){
	attackChance = ((double)foodHeld + (double)metalHeld + (double)medicalHeld)/((double)cargoCap*8);
	mvprintw(1, 11, "-Player Data-\t");
	mvprintw(2, 11,"Player name: %s\t\t", playerName.c_str());
	mvprintw(3, 11, "Credits: %i\t\t\t", creditsHeld);
	mvprintw(4, 11, "Chance to be attacked: %.2f%%", attackChance*100.0);
	mvprintw(5, 11, "\t\t\t\t");
	mvprintw(6, 11, "\t\t\t\t");
	mvprintw(7, 11, "\t\t\t\t");
}

/*
List information on the ship and cargo
*/
void showShipData(){
	mvprintw(1, 11, "-Ship Data-\t");
	mvprintw(2, 11, "Ship name: %s\t\t", shipName.c_str());
	mvprintw(3, 11, "Cargo capacity: %i/%i\t\t", (cargoCap-spaceLeft), cargoCap);
	mvprintw(4, 11, "Food held: %i\t\t\t\t\t", foodHeld);
	mvprintw(5, 11, "Metal held: %i\t\t\t", metalHeld);
	mvprintw(6, 11, "Medical held: %i\t\t\t", medicalHeld);
}

/*
List information concerning the local star system
*/
void showSystemData(solsystem *newS){
	mvprintw(1, 11, "-System Data-\t");
	mvprintw(2, 11, "System name: %s\t\t\t\t", newS->getName().c_str());
	mvprintw(3, 11, "Food: %i\t\t\t", newS->getFoodPrice());
	mvprintw(4, 11, "Metal: %i\t\t\t\t\t", newS->getMetalPrice());
	mvprintw(5, 11, "Medical: %i\t\t\t", newS->getMedicalPrice());
	mvprintw(6, 11, "\t\t\t\t");
	mvprintw(7, 11, "\t\t\t\t");
}

/*
Redraw the system list
*/
void updateSystemList(){
	for(int i = 0; i < numSystems; i++){
		if(list[i]->getIsCurrent()){
			attron(COLOR_PAIR(2) | A_REVERSE);
		}
		mvprintw(10+i, 1, list[i]->getName().c_str());
		attroff(A_REVERSE);
		attron(COLOR_PAIR(4));
	}
} 

/*
Validate the existence of a given system
*/
bool systemExists(string n){
	for(int i = 0; i < 4; i++){
		if((list[i]->getName().compare(n))==0) return true;
	}
	return false;
}

/*
Open a new window so we can jump to another system
*/
void openJumpDialog(){

	//Create input window
	WINDOW *jumpInput = create_newwin(5, 30, 9, 21);
	wborder(jumpInput, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wattron(jumpInput, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(jumpInput, 0, 0, "Enter a system: ");

	/*
	Temporarily enable input echoing and the keyboard buffer,
	since we're expecting a string and want it to be visible
	*/
	int tmpX = 0, tmpY = 1;
	wmove(jumpInput, tmpY, tmpX);
	curs_set(1);
	wrefresh(jumpInput);
	wattron(jumpInput, COLOR_PAIR(2) | A_BOLD);
	noraw();
	echo();

	/*
	As long as the user hasn't hit enter, keep taking input
	and writing it to a string
	*/
	char ch; string inSystem;
	while((ch=wgetch(jumpInput)) != KEY_ENTER){
		if(ch == 10) break;
		wmove(jumpInput, tmpY, tmpX);
		wprintw(jumpInput, &ch);
		wrefresh(jumpInput);
		tmpX++;
		inSystem+=ch;
	}

	/*
	Disable the keyboard buffer and echoing,
	and clear he window
	*/
	raw();
	noecho();
	mvwprintw(jumpInput, 0, 0, "\t\t\t");
	mvwprintw(jumpInput, 1, 0, "\t\t\t");
	mvwprintw(jumpInput, 2, 0, "\t\t\t");
	mvwprintw(jumpInput, 3, 0, "\t\t\t");
	curs_set(0);

	/*
	Validate the existence of the requested system.
	If it doesn't exist, yell at them.
	*/
	if(!systemExists(inSystem)){
		mvwprintw(jumpInput, 0, 0, "System doesn't exist!");
		wgetch(jumpInput);
		mvwprintw(jumpInput, 0, 0, "\t\t\t");
	}else{
		/*
		Otherwise, the system does exist and we can jump to it.
		Now update the current system. This is when pirates should spawn.
		*/
		for(int i = 0; i < 4; i++){
			if(list[i]->getName().compare(inSystem) == 0){
				current->setCurrent(FALSE);
				current = list[i];
				current->setCurrent(TRUE);
				showSystemData(current);
				updateSystemList();
				break;
			}
		}
	}
	
	destroy_win(jumpInput);
}                                                      

/*
Open the market window to buy things
*/
void openMarket(solsystem *tmp){
	
	/*
	Draw the initial market window
	*/
	WINDOW *market = create_newwin(13, 40, 9, 25);
	wattron(market, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(market, 1, 1, "-Market in system: %s-", tmp->getName().c_str());
	mvwprintw(market, 3, 1, "-Make a selection-");
	mvwprintw(market, 5, 1, "1)Food: %i", tmp->getFoodPrice());
	mvwprintw(market, 6, 1, "2)Metal: %i", tmp->getMetalPrice());
	mvwprintw(market, 7, 1, "3)Medical: %i", tmp->getMedicalPrice());
	mvwprintw(market, 10, 1, "Cargo space available: %i", spaceLeft);
	mvwprintw(market, 11, 1, "Credits held: %i", creditsHeld);

	/*
	Select a product to buy, make sure it was a valid choice
	*/
	char selection; int quant = 0; int unitPrice; int currentQuant;
	selection = wgetch(market);
	while(selection != '1' && selection != '2' && selection != '3'){
		selection = wgetch(market);
	}

	/*
	Define which product we picked, get its unit price
	*/
	if(selection == '1') { unitPrice = tmp->getFoodPrice(); currentQuant = foodHeld; }
	if(selection == '2') { unitPrice = tmp->getMetalPrice(); currentQuant = metalHeld; }
	if(selection == '3') { unitPrice = tmp->getMedicalPrice(); currentQuant = medicalHeld; }

	/*
	Square brackets increment and decrement quantity to purchase.
	Pressing 'q' will close this window.
	Each time we get a valid key press, redraw the relevant lines.
	*/
	mvwprintw(market, 9, 1, "[ %i ]", currentQuant);
	mvwprintw(market, 8, 1, "-Use [ and ] to specify a quantity-");
	char ch;
	while((ch = wgetch(market))!='q'){
		if(ch == ']' && spaceLeft != 0 && creditsHeld >= unitPrice){ spaceLeft--; creditsHeld-=unitPrice; currentQuant++;}//Buy
		if(ch == '[' && spaceLeft != cargoCap && currentQuant != 0) { spaceLeft++; creditsHeld+=unitPrice; currentQuant--;}//Sell
		mvwprintw(market, 9, 1, "[ %i ]\t", currentQuant);
		mvwprintw(market, 10, 1, "Cargo space available: %i\t", spaceLeft);
		mvwprintw(market, 11, 1, "Credits held: %i\t", creditsHeld);
		wrefresh(market);

		if(selection == '1') foodHeld = currentQuant;
		if(selection == '2') metalHeld = currentQuant;
		if(selection == '3') medicalHeld = currentQuant;
		showShipData();
	}

	/*
	Switch back to the needed color pair and
	clear the window
	*/
	wattron(market, COLOR_PAIR(4));
	mvwprintw(market, 1, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 3, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 5, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 6, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 7, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 8, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 9, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 10, 1, "\t\t\t\t\t\t ");
	mvwprintw(market, 11, 1, "\t\t\t\t\t\t ");
	destroy_win(market);
	
}       

/*
Create a window to display a short in-game manual
*/
void openManual(){
	WINDOW *manual = create_newwin(13, 50, 9, 25);
	wattron(manual, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(manual, 1, 1, "Manual - press any button to close");
	mvwprintw(manual, 2, 1, "1: Open dialog to jump to a new system.");
	mvwprintw(manual, 3, 1, "2: Open market for current system to buy cargo.");
	mvwprintw(manual, 4, 1, "3-5: Display data on player, ship, and system.");
	mvwprintw(manual, 5, 1, "Press e to quit the game.");
	mvwprintw(manual, 7, 1, "After jumping, there is a chance to be attacked");
	mvwprintw(manual, 8, 2, "by pirates.");
	mvwprintw(manual, 9, 1, "Your goal is to trade between systems and");
	mvwprintw(manual, 10, 2, "survive pirate attacks.");	
	mvwprintw(manual, 11, 1, "A better manual may be found in ./README.md");
	

	/*
	Close the window after any key is pressed,
	clear the text with an assload of whitespace
	*/
	wgetch(manual);
	mvwprintw(manual, 1, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 2, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 3, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 4, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 5, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 6, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 7, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 8, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 9, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 10, 1, "\t\t\t\t\t\t ");
	mvwprintw(manual, 11, 1, "\t\t\t\t\t\t ");
	destroy_win(manual);
}

/*
Open a file stream to load data from a save file
*/
void loadSave(){
	ifstream load("player.sav");
	getline(load, playerName);
	string tmp; getline(load, tmp); creditsHeld = stoi(tmp);
	getline(load, shipName);
	getline(load, tmp); cargoCap = stoi(tmp);
	getline(load, tmp); spaceLeft = stoi(tmp);
	getline(load, tmp); foodHeld = stoi(tmp);
	getline(load, tmp); metalHeld = stoi(tmp);
	getline(load, tmp); medicalHeld = stoi(tmp);
	load.close();
}

/*
Open a file stream to save player data to a file
*/
void saveData(){
	ofstream save("player.sav", ios::binary);
	save << playerName << endl;
	save << creditsHeld << endl;
	save << shipName << endl;
	save << cargoCap << endl;
	save << spaceLeft << endl;
	save << foodHeld << endl;
	save << metalHeld << endl;
	save << medicalHeld << endl;
	save.close();

}

int main(int argc, char **argv){
	loadSave();
	
	/*
	Initialize system objects and add them
	to list array for better referencing
	*/
	Beta->setCurrent(true);
	for(int i = 0; i < 5; i++) list[i] = new solsystem();
	list[0] = Alpha;
	list[1] = Beta;
	list[2] = Delta;
	list[3] = Gamma;
	list[4] = current;

	/*
	Initialize the screen, disable
	echoing and the keyboard buffer,
	and get the dimensions of the window
	*/
	int winY, winX;
	initscr();
	noecho();
	raw();
	keypad(stdscr, TRUE);
	start_color();
	getmaxyx(stdscr, winY, winX);
	curs_set(0);	

	/*
	Define color pairs, draw the border
	*/
	init_pair(1, COLOR_WHITE, COLOR_WHITE);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1) | A_REVERSE | A_BOLD);
	border(' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	attroff(A_REVERSE);

	/*
	Draw the functions list
	*/
	attron(COLOR_PAIR(2) | A_BOLD);
	mvprintw(1, 1, "1: jump");
	mvprintw(2, 1, "2: market");
	mvprintw(3, 1, "3: player");
	mvprintw(4, 1, "4: ship");
	mvprintw(5, 1, "5: system");
	mvprintw(22, 1, "M: manual");

	/*
	Draw the borders between different
	parts of the screen
	*/
	move(8, 1);
	attron(COLOR_PAIR(3) | A_BOLD);
	hline('-', 78);
	move(1, 10);
	vline('|', 7);
	
	/*
	Set attributes for the systems list,
	among other things
	*/
	attron(COLOR_PAIR(4) | A_UNDERLINE);
	mvprintw(9, 1, "-Available systems-");
	attroff(A_UNDERLINE);
	attron(COLOR_PAIR(3) | A_BOLD);
	vline('|', 14);
	attron(COLOR_PAIR(4));

	/*
	List all available systems
	*/
	updateSystemList();

	/*
	Main input loop. If we get an 'e',
	quit the game.
	*/
	char ch;
	while((ch = getch()) != 'e'){
		if(ch == '1') openJumpDialog();
		if(ch == '2') openMarket(current);
		if(ch == '3') showPlayerData();
		if(ch == '4') showShipData();
		if(ch == '5') showSystemData(current);
		if(ch == 'm' || ch == 'M') openManual();
	}
		
	/*
	Ending the program and stopping curses mode,
	clean up objects in memory, save player data
	*/
	endwin();
	delete Alpha;
	delete Beta;
	delete Delta;
	delete Gamma;
	delete list;
	saveData();
	return 0;
}                
