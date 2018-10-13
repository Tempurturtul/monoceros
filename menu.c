/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: menu implementation file
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "menu.h"

int mainMenu() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	WINDOW *menuW = newwin(maxY - 4, 27, 4, maxX/2 - 1 - 27/2);
	wattron(menuW, A_BOLD | A_DIM);

	const char *menuStr = ""
		"Press number for selection:\n"
		"(1) single player\n"
		"(2) multiplayer\n"
		"(3) high scores\n"
		"(4) exit";

	mvwprintw(menuW, 0, 0, menuStr);

	wrefresh(w);
	wrefresh(menuW);
	
	char input = ' ';
	while (input == ' ') {
		input = wgetch(menuW);
	}

	delwin(menuW);
	delwin(w);

	// 48 is the ASCII value for 0.
	return (input-48);
}

void dispScores() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	WINDOW *scoresW = newwin(maxY - 2, 26, 2, maxX/2 - 1 - 26/2);
	wattron(w, A_BOLD | A_DIM);
	wattron(scoresW, A_BOLD | A_DIM);

	char *title = ""
		"RANK  SCORE    NAME      \n";
	char *first = ""
		"1ST   9999999  NAGATE    \n";
	char *rest = ""
		"2ND   9999999  NAGATE    \n"
		"3RD   9999999  NAGATE    \n"
		"4TH   9999999  NAGATE    \n"
		"5TH   0742400  HOSHIJIRO \n"
		"6TH   0239115  IZANA     \n";

	wattron(scoresW, COLOR_PAIR(4));
	mvwprintw(scoresW, 0, 0, title);
	wattroff(scoresW, COLOR_PAIR(4));
	wattron(scoresW, COLOR_PAIR(3));
	mvwprintw(scoresW, 2, 0, first);
	wattroff(scoresW, COLOR_PAIR(3));
	mvwprintw(scoresW, 3, 0, rest);

	wrefresh(w);
	wrefresh(scoresW);

	// Wait for any input.
	wgetch(w);

	delwin(scoresW);
	delwin(w);
}

void loadingScreen(const char *text, int secondsLoading) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	wattron(w, A_BOLD | A_DIM);

	char *loadingText = malloc(sizeof(char) * (strlen(text) + 4));
	strcpy(loadingText, text);

	switch (secondsLoading % 4) {
		case 0:
			strcat(loadingText, "   ");
			break;
		case 1:
			strcat(loadingText, ".  ");
			break;
		case 2:
			strcat(loadingText, ".. ");
			break;
		default:
			strcat(loadingText, "...");
			break;
	}

	mvwprintw(w, maxY/2 - 1, maxX/2 - strlen(text)/2 - 1, loadingText);
	wrefresh(w);

	free(loadingText);
	delwin(w);
}
