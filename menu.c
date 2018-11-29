/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt, Chad Erdwins
** Date:  13 OCT 2018
** Description: menu implementation file
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>

#include "menu.h"
#include "scores.h"

#define MAX_SCORES_OFFSET 990

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

WINDOW *deathScreen(WINDOW *w, int finalScore, char name[11]) {
	const char *deathMsg = "YOU DIED x_x";
	const char *scorePrefix = "Final Score: ";
	const char *namePrompt = "Name: ";
	int deathMsgLen = strlen(deathMsg);
	int scoreMsgLen = strlen(scorePrefix) + 7; // 7 digit scores
	int namePromptLen = strlen(namePrompt);
	int nameMsgLen = namePromptLen + 10; // 10 character names.

	if (w == NULL) {
		// Need to initialize window.
		int maxX, maxY;
		getmaxyx(stdscr, maxY, maxX);

		wclear(stdscr);

		char scoreMsg[scoreMsgLen+1];
		sprintf(scoreMsg, "%s%07d", scorePrefix, finalScore);

		w = newwin(5, scoreMsgLen, maxY/2 - 1 - 5/2, maxX/2 - 1 - scoreMsgLen/2);
		wattron(w, A_BOLD | A_DIM);

		// Print death message.
		wattron(w, COLOR_PAIR(2)); // Red font.
		mvwprintw(w, 0, scoreMsgLen/2 - 1 - deathMsgLen/2, deathMsg); // Note score message must be wider than death message.
		wattroff(w, COLOR_PAIR(2));

		// Print final score.
		mvwprintw(w, 2, 0, scoreMsg);

		// Print name prompt.
		mvwprintw(w, 4, scoreMsgLen/2 - 1 - nameMsgLen/2, namePrompt);

		wrefresh(stdscr);
		wrefresh(w);
	}

	// Update displayed name only.
	mvwprintw(w, 4, scoreMsgLen/2 - 1 - nameMsgLen/2 + namePromptLen, "%-10s", name);

	return w;
}

bool handleDeathScreenInput(int input, char name[11]) {
	int chars = strlen(name);

	if (input == '\n' || input == '\r') {
		// Enter key.
		if (chars > 0) {
			// All done.
			return false;
		}
	} else if (input == KEY_BACKSPACE) {
		// Backspace key.
		if (chars > 0) {
			name[chars-1] = '\0';
		}
	} else if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122)) {
		// A-Z or a-z.
		if (chars < 10) {
			// Convert lowercase to uppercase.
			if (input > 90) {
				input -= 32;
			}
			name[chars] = input;
			name[chars+1] = '\0';
		}
	}

	// Need more input.
	return true;
}

WINDOW *gameHeader(WINDOW *w, struct gameState *state, struct levelData *level) {
	if (w == NULL) {
		// Need to initialize window.
		w = newwin(state->titleSize, state->maxX, 0, 0);
	}

	//wclear(w);
	werase(w);
	box(w, 0, 0);

	bool debugging = false; // Change for debugging info (requires titleSize >= 3).

	if (debugging) {
		//mvwprintw(w, 0, 1, "xLoc:%f",state->allSprites->spriteArr[0]->xLoc);
		//mvwprintw(w, 1, 1, "xVel:%f",state->allSprites->spriteArr[0]->xVel);
		//mvwprintw(w, 2, 1, "xMax:%i",state->maxX);

		//mvwprintw(w, 0, 20, "numEnemies:%i",level->numEnemies);
		mvwprintw(w, 1, 0, "numSprites:%i",state->allSprites->numSprites);
		//mvwprintw(w, 2, 20, "yVel:%f",state->allSprites->spriteArr[0]->xAcc);

		mvwprintw(w, 1, 20, "numEffects:%i",state->allEffects->numEffects);
		// mvwprintw(w, 1, 40, "radius gnd:%f",lib->allSprites->spriteArr[gnd1]->radius);
		// mvwprintw(w, 2, 40, "xCoM gnd:%f",lib->allSprites->spriteArr[gnd1]->xCoM);
		//mvwprintw(w, 2, 40, "numDisps eff6:%i",lib->allEffects->effectArr[6]->numDisps);

		mvwprintw(w, 1, 60, "AMMO:%i",(int)state->allSprites->spriteArr[0]->isShooter);


		mvwprintw(w, 0, state->maxX - 15, "time: %.1f", round(state->time*10)/10);
		mvwprintw(w, 1, state->maxX - 15, "SCORE: %i", state->score);
		mvwprintw(w, 2, state->maxX - 15, "LEVEL: %i", level->currLevel);
	} else {
		// Level
		mvwprintw(w, 1, 2, "LEVEL %i", level->currLevel);
		// Ammo
		mvwprintw(w, 1, state->maxX / 2 - 1 - 8, "AMMO: %i", (int)state->allSprites->spriteArr[0]->isShooter);
		// Score
		mvwprintw(w, 1, state->maxX - 12, "SCORE: %i", state->score);
	}

	//wrefresh(w);
	wnoutrefresh(w);

	return w;
}