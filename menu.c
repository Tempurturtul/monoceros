/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: menu implementation file
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "menu.h"

int getMenu() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *menu = newwin(maxY, maxX, 0, 0);

	char chInput = ' ';
	const char *selectionStr = "Press number for selection:\n(1) single player\n(2) dual player\n(3) view high score\n(4) exit";
	keypad(menu, TRUE);

	wclear(menu);

	mvwprintw(menu, 0, 0, selectionStr);

	wrefresh(menu);
	
	while (needInput(menu, &chInput)) {
		mvwprintw(menu, 0, 0, selectionStr);
		wrefresh(menu);
	}

	delwin(menu);

	// 48 is the ascii value for 0
	return (chInput-48);
}

int centerText(WINDOW* window, const char * text) {
	int maxX, maxY;
	getmaxyx(window, maxY, maxX);
	
	return (maxX/2 - strlen(text));
}

int needInput(WINDOW * window, char * chInput_in) {
	*chInput_in = wgetch(window);
	// logic for testing a valid selection
	if (*chInput_in == '1' ||
		*chInput_in == '2' ||
		*chInput_in == '3' ||
		*chInput_in == '4') {
			return 0;
		}
	// otherwise return 1 (still need input)
	return 1;
}

void dispScores() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *scores = newwin(maxY, maxX, 0, 0);

	const char * gameStr = "showing those high scores!";
	int startX = centerText(scores, gameStr);

	wclear(scores);
	mvwprintw(scores, 5,startX, gameStr);
	wrefresh(scores);
	sleep(3);

	delwin(scores);
}