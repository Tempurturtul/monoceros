/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt, Chad Erdwins
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
#include "tcp_client.h"

void startCurses();
void endCurses();

int main(int argc, char *argv[]) {
	startCurses();
	srand(time(0));

	int menuChoice = 0;
	while (menuChoice != 4) {
		switch (menuChoice) {
			case 1:
				//playGame(state, lib, level);
				single_player_instructions();
				playGameSingle();
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
	init_pair(7, COLOR_BLACK, COLOR_BLACK);
	init_pair(8, COLOR_GREEN, COLOR_BLACK);
	init_pair(9, COLOR_YELLOW, COLOR_WHITE);
	init_pair(10, COLOR_RED, COLOR_YELLOW);
	init_pair(11, COLOR_YELLOW, COLOR_RED);
	init_pair(12, COLOR_BLACK, COLOR_GREEN);
	init_pair(13, COLOR_BLUE, COLOR_GREEN);
	init_pair(14, COLOR_RED, COLOR_WHITE);
	// sky  motif
	init_pair(15, COLOR_RED, COLOR_CYAN);
	init_pair(16, COLOR_WHITE, COLOR_CYAN);
	init_pair(17, COLOR_BLACK, COLOR_CYAN);
	init_pair(18, COLOR_GREEN, COLOR_CYAN);
	init_pair(19, COLOR_BLUE, COLOR_CYAN);
	// ammo crates
	init_pair(20, COLOR_YELLOW, COLOR_RED);
	init_pair(21, COLOR_RED, COLOR_YELLOW);
	// sky motif again
	init_pair(22, COLOR_YELLOW, COLOR_CYAN);
}

void endCurses() {
	endwin();
}
