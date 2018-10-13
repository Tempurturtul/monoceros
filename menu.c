/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: menu implementation file
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

#include "menu.h"

int mainMenu() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	WINDOW *menuW = newwin(maxY - 4, 27, 4, maxX/2 - 27/2);

	const char *menuStr = "Press number for selection:\n(1) single player\n(2) multiplayer\n(3) high scores\n(4) exit";

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
	WINDOW *scoresW = newwin(maxY - 2, 15, 2, maxX/2 - 15/2);

	mvwprintw(w, maxY-1, 0, "press (q) to quit");
	mvwprintw(scoresW, 0, 0, "  High Scores  \n===============\n\nNAGATE  9999999\nNAGATE  9999999\nNAGATE  9999999\nNAGATE  9999999\nHOSHIJ  0742400\nIZANA   0239115");

	char c = ' ';
	while (c != 'q') {
		wrefresh(w);
		wrefresh(scoresW);
		c = wgetch(w);
	}

	delwin(scoresW);
	delwin(w);
}

void loadingScreen(int startUnixtime) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);

	int currentUnixtime = (int)time(NULL);

	char *loadingText;

	// Uncomment while-loop and time updates to preview an 8 second loading screen.

	// startUnixtime = (int)time(NULL);
	// while (currentUnixtime - startUnixtime < 8) {
		switch ((currentUnixtime - startUnixtime) % 4) {
			case 0:
				loadingText = "loading   ";
				break;
			case 1:
				loadingText = "loading.  ";
				break;
			case 2:
				loadingText = "loading.. ";
				break;
			case 3:
				loadingText = "loading...";
				break;
		}

		mvwprintw(w, maxY/3, maxX/2 - strlen(loadingText)/2, loadingText);
		wrefresh(w);

	// 	currentUnixtime = (int)time(NULL);
	// }

	delwin(w);
}
