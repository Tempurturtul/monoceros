/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: main program file for Monoceros CS467 project

NOTES
1. setup enum for menu options?
2. do you want to do ncurses init and winow stuff here? or specific to game / high scores / exit?


*******************************************************************************************/
//#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "menu.h"
#include "gamePlay.h"


int main(int argc, char *argv[]) { 
	// specific for controlling window sizes
	int parent_x, parent_y, new_x, new_y; 
	int title_size = 3; 

	// for game interaction
	int menuChoice = 0;

	// initialize curses (move this to it's own business i think)
	initscr(); 
	noecho(); 
	curs_set(FALSE);	// get our maximum window dimensions 
	start_color();		// init colors
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	
	getmaxyx(stdscr, parent_y, parent_x); // set up initial windows 
	WINDOW *title = newwin(title_size, parent_x, 0, 0); 
	WINDOW *action = newwin(parent_y-title_size, parent_x, title_size, 0); // draw to our windows 
//	mvwin(title, parent_y-title_size, parent_x);
//	wclear(stdscr);
//	wclear(title);
//	wclear(action);
	
	char chInput;
	while (menuChoice != 4) {
	
		//menuChoice = getMenu(title, action);
		
		if (menuChoice == 1) {
			playGame(title, action);
		}
		else if (menuChoice == 2) {
			waitQueue(title, action);
		}
		else if (menuChoice == 3) {
			dispScores(title, action);
		}
		
		menuChoice = getMenu(title, action);
	

	}
	


	// clean up
	delwin(title); 
	delwin(action); 
	
	endwin();
	
	return 0;
}
