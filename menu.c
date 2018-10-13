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

	WINDOW *scoresW = newwin(maxY - 2, 25, 2, maxX/2 - 25/2);

	char *title = ""
		"RANK  SCORE    NAME      \n";
	char *scores = ""
		"1ST   9999999  NAGATE    \n"
		"2ND   9999999  NAGATE    \n"
		"3RD   9999999  NAGATE    \n"
		"4TH   9999999  NAGATE    \n"
		"5TH   0742400  HOSHIJIRO \n"
		"6TH   0239115  IZANA     \n";

	mvwprintw(w, maxY-1, 0, "press (q) to quit");
	mvwprintw(scoresW, 0, 0, title);
	mvwprintw(scoresW, 2, 0, scores);

	wrefresh(w);
	wrefresh(scoresW);

	char c = ' ';
	while (c != 'q') {
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
