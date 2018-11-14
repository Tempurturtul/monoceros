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
#include "scores.h"

#define MAX_SCORES_OFFSET 990

int mainMenu() {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	WINDOW *menuW = newwin(maxY - 4, 27, 4, maxX/2 - 1 - 27/2);
	wattron(menuW, A_BOLD | A_DIM);

	const char *menuTitle = "=== MONOCEROS ===";
	const char *menuStr = ""
		"Press number for selection:\n"
		"(1) single player\n"
		"(2) multiplayer\n"
		"(3) high scores\n"
		"(4) exit";

	wattron(menuW, COLOR_PAIR(5));
	wattroff(menuW, A_DIM);
	mvwprintw(menuW, 0, 27/2 + 1 - strlen(menuTitle)/2, menuTitle);
	wattron(menuW, A_DIM);
	wattroff(menuW, COLOR_PAIR(5));

	mvwprintw(menuW, 2, 0, menuStr);

	wrefresh(w);
	wrefresh(menuW);

	int input = wgetch(menuW);

	delwin(menuW);
	delwin(w);

	// 48 is the ASCII value for 0.
	return (input-48);
}

void dispScores() {
	// Uncomment to generate scores for debugging:
	/*
	struct highscore s;
	s.score = 9999999;
	strcpy(s.name, "NAGATE");
	putScore(s);
	s.score = 2805998;
	strcpy(s.name, "HOSHIJIRO");
	putScore(s);
	s.score = 835573;
	strcpy(s.name, "IZANA");
	putScore(s);
	*/

	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);
	keypad(w, true);

	int lineLength = 26; // 5 for rank + 2 for spacing, 7 for score + 2 for spacing, + 10 for name.

	WINDOW *scoresW = newwin(12, lineLength, maxY/2 - 1 - 12/2, maxX/2 - 1 - lineLength/2);
	wattron(scoresW, A_BOLD | A_DIM);

	char line[lineLength+1];
	char rank[6];
	char rankSuffix[3];

	struct highscore scores[10];
	int offset = 0;
	int i;
	int input;

	while (1) {
		wclear(w);
		wclear(scoresW);

		// Header in red.
		wattron(scoresW, COLOR_PAIR(2));
		wprintw(scoresW, "RANK  SCORE    NAME");
		wattroff(scoresW, COLOR_PAIR(2));

		// Get scores.
		getScores(scores, 10, offset);

		// Display scores.
		for (i = 0; i < 10; i++) {
			if (strcmp(scores[i].name, "") == 0) {
				if (i == 0) {
					mvwprintw(scoresW, 2, 0, "(nothing here)");
				}
				break;
			}

			// Figure out rank.
			switch (i+1+offset % 10) {
				case 1:
					strcpy(rankSuffix, "ST");
					break;
				case 2:
					strcpy(rankSuffix, "ND");
					break;
				case 3:
					strcpy(rankSuffix, "RD");
					break;
				default:
					strcpy(rankSuffix, "TH");
			}
			if (i+1+offset == 11 || i+1+offset == 12 || i+1+offset == 13) {
				strcpy(rankSuffix, "TH");
			}
			sprintf(rank, "%d%s", i+1+offset, rankSuffix);

			// Rank space-padded to 5 characters, scores zero-padded to 7 digits, and name space-padded to 10 characters and left-aligned.
			sprintf(line, "%5s  %07d  %-10s", rank, scores[i].score, scores[i].name);

			if (i == 0 && offset == 0) {
				// Give first place yellow font color.
				wattron(scoresW, COLOR_PAIR(3));
				mvwprintw(scoresW, i+2, 0, line);
				wattroff(scoresW, COLOR_PAIR(3));
			} else if (i < 3 && offset == 0) {
				// Give second and third place cyan font color.
				wattron(scoresW, COLOR_PAIR(4));
				mvwprintw(scoresW, i+2, 0, line);
				wattroff(scoresW, COLOR_PAIR(4));
			} else {
				// Remaining places get no special font color.
				mvwprintw(scoresW, i+2, 0, line);
			}
		}

		// Display navigation hints.
		if (offset > 0) {
			mvwprintw(w, maxY-2, 2, "<--");
		}
		if (scores[9].score > 0 && offset < MAX_SCORES_OFFSET) {
			mvwprintw(w, maxY-2, maxX-5, "-->");
		}

		wrefresh(w);
		wrefresh(scoresW);

		// Wait for input.
		input = wgetch(w);
		if (input == KEY_LEFT) {
			offset -= 10;
			if (offset < 0) {
				offset = 0;
			}
		} else if (input == KEY_RIGHT) {
			if (scores[9].score > 0 && offset < MAX_SCORES_OFFSET) {
				offset += 10;
			}
		} else {
			break;
		}
	}

	delwin(scoresW);
	delwin(w);
}

void messageScreen(const char *text) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);

	wattron(stdscr, A_BOLD | A_DIM);

	wclear(stdscr);
	mvwprintw(stdscr, maxY/2 - 1, maxX/2 - strlen(text)/2 - 1, text);
	wrefresh(stdscr);

	wattroff(stdscr, A_BOLD | A_DIM);
}

void deathScreen(int finalScore, char nameBuffer[11]) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);

	char scoreMsg[13+7+1]; // 13 character prefix including space, 7 digit score, null-terminator.
	sprintf(scoreMsg, "Final Score: %07d", finalScore);

	WINDOW *msgW = newwin(5, strlen(scoreMsg), maxY/2 - 1 - 5/2, maxX/2 - 1 - strlen(scoreMsg)/2);
	wattron(msgW, A_BOLD | A_DIM);

	// Print death message.
	const char *deathMsg = "YOU DIED x_x";
	wattron(msgW, COLOR_PAIR(2)); // Red font.
	mvwprintw(msgW, 0, strlen(scoreMsg)/2 - 1 - strlen(deathMsg)/2, deathMsg); // Note score message must be wider than death message.
	wattroff(msgW, COLOR_PAIR(2));

	// Print final score.
	mvwprintw(msgW, 2, 0, scoreMsg);

	wrefresh(w);
	wrefresh(msgW);

	// Prepare name prompt.
	char namePrompt[6+10+1]; // "Name: " (6) + input (10) + null terminator (1).
	// Make sure name buffer contains an empty string.
	nameBuffer[0] = '\0';

	// Print initial name prompt.
	sprintf(namePrompt, "Name: %s", nameBuffer);
	wattron(msgW, A_BLINK);
	mvwprintw(msgW, 4, strlen(scoreMsg)/2 - 1 - 16/2, namePrompt);
	wattroff(msgW, A_BLINK);
	wrefresh(msgW);

	// Wait half a second in case the user is spamming input post-death.
	sleep(0.5);

	keypad(w, TRUE);
	int input;
	int chars = 0;
	bool collectInput = true;
	while (collectInput) {
		input = wgetch(w);

		if (input == '\n' || input == '\r') {
			// Enter key.
			if (chars > 0) {
				collectInput = false;
			}
		} else if (input == KEY_BACKSPACE) {
			// Backspace key.
			if (chars > 0) {
				nameBuffer[chars-1] = ' ';
				chars--;
			}
		} else if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122)) {
			// A-Z or a-z.
			if (chars < 10) {
				// Convert lowercase to uppercase.
				if (input > 90) {
					input -= 32;
				}
				nameBuffer[chars] = input;
				nameBuffer[chars+1] = '\0';
				chars++;
			}
		}

		sprintf(namePrompt, "Name: %s", nameBuffer);
		mvwprintw(msgW, 4, strlen(scoreMsg)/2 - 1 - 16/2, namePrompt);
		wrefresh(msgW);
	}

	delwin(msgW);
	delwin(w);
}
