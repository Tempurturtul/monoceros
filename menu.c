/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt, Chad Erdwins
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
	WINDOW *menuW = newwin(maxY - 4, 27, 18, maxX/2 - 1 - 27/2);
	wattron(menuW, A_BOLD | A_DIM);

	printf("%d\n", maxX);
	const char *menuTitle1 = "  __    __     _____     __   __     _____     _____    _____  __ __     _____   ______  \n";
	const char *menuTitle2 = " /_/\\  /\\_\\   ) ___ (   /_/\\ /\\_\\   ) ___ (   /\\ __/\\ /\\_____\\/_/\\__/\\  ) ___ ( / ____/\\ \n";
	const char *menuTitle3 = " ) ) \\/ ( (  / /\\_/\\ \\  ) ) \\ ( (  / /\\_/\\ \\  ) )__\\/( (_____/) ) ) ) )/ /\\_/\\ \\) ) __\\/ \n";
	const char *menuTitle4 = "/_/ \\  / \\_\\/ /_/ (_\\ \\/_/   \\ \\_\\/ /_/ (_\\ \\/ / /    \\ \\__\\ /_/ /_/_// /_/ (_\\ \\\\ \\ \\   \n";
	const char *menuTitle5 = "\\ \\ \\\\// / /\\ \\ )_/ / /\\ \\ \\   / /\\ \\ )_/ / /\\ \\ \\_   / /__/_\\ \\ \\ \\ \\\\ \\ )_/ / /_\\ \\ \\  \n";
	const char *menuTitle6 = " )_) )( (_(  \\ \\/_\\/ /  )_) \\ (_(  \\ \\/_\\/ /  ) )__/\\( (_____\\)_) ) \\ \\\\ \\/_\\/ /)____) ) \n";
	const char *menuTitle7 = " \\_\\/  \\/_/   )_____(   \\_\\/ \\/_/   )_____(   \\/___\\/ \\/_____/\\_\\/ \\_\\/ )_____( \\____\\/  \n";


	const char *menuStr = ""
		"Press number for selection:\n"
		"(1) single player\n"
		"(2) multiplayer\n"
		"(3) high scores\n"
		"(4) exit";

	wattron(menuW, COLOR_PAIR(4));
	wattroff(menuW, A_DIM);
	if (maxX > 89) {
		wattron(w, COLOR_PAIR(4));
		mvwprintw(w, 8, maxX/2 + 1 - 89/2, menuTitle1);
		mvwprintw(w, 9, maxX/2 + 1 - 89/2, menuTitle2);
		mvwprintw(w, 10, maxX/2 + 1 - 89/2, menuTitle3);
		mvwprintw(w, 11, maxX/2 + 1 - 89/2, menuTitle4);
		mvwprintw(w, 12, maxX/2 + 1 - 89/2, menuTitle5);
		mvwprintw(w, 13, maxX/2 + 1 - 89/2, menuTitle6);
		mvwprintw(w, 14, maxX/2 + 1 - 89/2, menuTitle7);
	} else {
		mvwprintw(menuW, 2, 27/2 + 1 - 16/2, "=== Monoceros ===");
	}

	if (maxY < 8) {
		/* code */
	}

	wattron(menuW, A_DIM);
	wattroff(menuW, COLOR_PAIR(4));

	mvwprintw(menuW, 3, 0, menuStr);

	// TODO: Remove when option 5 is removed.
	mvwprintw(menuW, 13, 0, "(5) preview death screen");

	wrefresh(w);
	wrefresh(menuW);

	int input = wgetch(menuW);

	delwin(menuW);
	delwin(w);

	// 48 is the ASCII value for 0.
	return (input-48);
}

// Arguments must be sorted by score descending and properly aligned with one another.
// For example, scores[0] and names[0] must be the highest score and corresponding player's name.
void dispScores(int scores[10], char names[10][11]) {
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *w = newwin(maxY, maxX, 0, 0);

	int lineLength = 25; // 3 for rank + 3 for spacing, 7 for score + 2 for spacing, 10 for name.

	WINDOW *scoresW = newwin(12, lineLength, maxY/2 - 1 - 12/2, maxX/2 - 1 - lineLength/2);
	wattron(scoresW, A_BOLD | A_DIM);

	// Header in red.
	wattron(scoresW, COLOR_PAIR(2));
	wprintw(scoresW, "RANK  SCORE    NAME");
	wattroff(scoresW, COLOR_PAIR(2));

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
		} else if (i < 3) {
			// Give second and third place cyan font color.
			wattron(scoresW, COLOR_PAIR(4));
			mvwprintw(scoresW, i+2, 0, line);
			wattroff(scoresW, COLOR_PAIR(4));
		} else {
			// Remaining places get no special font color.
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
