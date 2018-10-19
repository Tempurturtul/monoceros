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

// Arguments must be sorted by score descending and properly aligned with one another.
// For example, scores[0] and names[0] must be the highest score and corresponding player's name.
void dispScores(int scores[10], char *names[10]) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);

	int lineLength = 25; // 3 for rank + 3 for spacing, 7 for score + 2 for spacing, 10 for name.

	WINDOW *scoresW = newwin(12, lineLength, 2, maxX/2 - 1 - lineLength/2);
	wattron(scoresW, A_BOLD | A_DIM);

	// Title.
	wattron(scoresW, COLOR_PAIR(4));
	wprintw(scoresW, "RANK  SCORE    NAME");
	wattroff(scoresW, COLOR_PAIR(4));

	char *ranks[10] = {
		"1ST",
		"2ND",
		"3RD",
		"4TH",
		"5TH",
		"6TH",
		"7TH",
		"8TH",
		"9TH",
		"10TH"
	};
	char line[lineLength+1];

	// First through tenth place.
	int i;
	for (i = 0; i < 10; i++) {
		// Don't display zero score.
		if (scores[i] == 0) {
			break;
		}

		// Rank space-padded to 4 characters, scores zero-padded to 7 digits, and name space-padded to 10 characters and left-aligned.
		sprintf(line, "%4s  %07d  %-10s", ranks[i], scores[i], names[i]);

		if (i == 0) {
			// Give first place yellow font color.
			wattron(scoresW, COLOR_PAIR(3));
			mvwprintw(scoresW, i+2, 0, line);
			wattroff(scoresW, COLOR_PAIR(3));
		} else {
			mvwprintw(scoresW, i+2, 0, line);
		}
	}

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

void deathScreen(int finalScore) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	wattron(w, A_BOLD | A_DIM);

	const char *deathMsg = "YOU DIED x_x";
	wattron(w, COLOR_PAIR(2)); // Red on black.
	mvwprintw(w, maxY/2 - 2, maxX/2 - 1 - strlen(deathMsg)/2, deathMsg);
	wattroff(w, COLOR_PAIR(2));

	const char *scoreMsgPrefix = "Final Score: ";
	char scoreMsg[strlen(scoreMsgPrefix)+7];
	sprintf(scoreMsg, "%s%07d", scoreMsgPrefix, finalScore); // Scores displayed as 7 digits with padding zeros (0001234).

	WINDOW *scoreW = newwin(1, strlen(scoreMsg), maxY/2, maxX/2 - 1 - strlen(scoreMsg)/2);
	wattron(scoreW, A_BOLD | A_DIM);

	wprintw(scoreW, scoreMsg);

	wrefresh(w);
	wrefresh(scoreW);

	// Wait one second then allow exit with any input.
	sleep(1);
	wgetch(w);

	delwin(scoreW);
	delwin(w);
}
