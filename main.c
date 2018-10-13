/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: main program file for Monoceros CS467 project

NOTES
1. setup enum for menu options?

*******************************************************************************************/
//#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "menu.h"
#include "gamePlay.h"

void startCurses();
void endCurses();

int main(int argc, char *argv[]) { 
	startCurses();

	int menuChoice = 0;

	while (menuChoice != 4) {
		switch (menuChoice) {
			case 1:
				playGame();
				break;
			case 2:
				waitQueue();
				break;
			case 3:
				dispScores();
				break;
		}

		menuChoice = getMenu();
	}

	endCurses();

	return 0;
}

void startCurses() {
	// Initialize curses.
	initscr(); 
	noecho(); 
	curs_set(FALSE);	// get our maximum window dimensions 
	start_color();		// init colors
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
}

void endCurses() {
	endwin();
}
