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
	srand(time(0));

	// TODO: Remove when we have real scores to pass to dispScores().
	int scores[10] = { 9999999, 9999999, 9999999, 9999999, 9999999, 6718703, 103580, 0, 0, 0 };
	char names[10][11] = { "NAGATE", "NAGATE", "NAGATE", "NAGATE", "NAGATE", "HOSHIJIRO", "IZANA", "", "", "" };

	// TODO: Remove when the death screen is moved somewhere else.
	char playerName[11]; // Filled with the player's name via the death screen.

	int menuChoice = 0;
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
				// TODO: Read scores and names from high scores file.
				dispScores(scores, names);
				break;
			case 5:
				// TODO: Remove when we have the death screen used somewhere.
				deathScreen(34230, playerName);
				// TODO: Save score and name to high scores file.
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
	init_pair(7, COLOR_BLACK, COLOR_BLACK);
	init_pair(8, COLOR_GREEN, COLOR_BLACK);
	init_pair(9, COLOR_YELLOW, COLOR_WHITE);
	init_pair(10, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(11, COLOR_RED, COLOR_RED);
	init_pair(12, COLOR_GREEN, COLOR_GREEN);
}

void endCurses() {
	endwin();
}
