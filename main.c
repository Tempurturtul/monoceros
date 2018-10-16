/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: main program file for Monoceros CS467 project

NOTES
1. setup enum for menu options?

*******************************************************************************************/
//#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#include "menu.h"
#include "gamePlay.h"
#include "sprites.h"

void startCurses();
void endCurses();

int main(int argc, char *argv[]) { 
	startCurses();

	int menuChoice = 0;
	// do you want this here? i'm starting to think you do not
/*	struct library * lib = malloc(sizeof(struct library));
	struct gameState * state = malloc(sizeof(struct gameState));
	struct levelData * level = malloc(sizeof(struct levelData));
	initGame(state, lib, level);
*/
	while (menuChoice != 4) {
		switch (menuChoice) {
			case 1:
				//playGame(state, lib, level);
				playGame();
				break;
			case 2:
				waitQueue();
				break;
			case 3:
				dispScores();
				break;
		}

		menuChoice = mainMenu();
	}

	endCurses();

	return 0;
}

void startCurses() {
	// Initialize curses.
	initscr(); 

	// Require color support.
	if (has_colors() == FALSE) {
		endwin();
		printf("Terminal must support color.\n");
		exit(1);
	}

	// TODO Use raw() or cbreak() to get unbuffered user input?

	// Don't echo user input.
	noecho(); 

	// Hide the cursor.
	curs_set(0);

	// Initialize colors.
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);
	init_pair(6, COLOR_GREEN, COLOR_BLUE);
}

void endCurses() {
	endwin();
}
