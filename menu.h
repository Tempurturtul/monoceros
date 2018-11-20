/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: menu header file
*******************************************************************************************/
#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <ncurses.h>
#include <arpa/inet.h>

#include "interfaces.h"

// Displays main menu options and returns player number selection.
int mainMenu();

// Displays up to 10 scores and names.
void dispScores();

// Displays the given text centered on stdscr.
void messageScreen(const char *text);

// Displays the player's final score and name (max of 7 digits and 10 characters + null terminator).
// The window returned from the function should be passed back in as the first argument on subsequent calls. As long as the
// first argument isn't NULL no new windows will be created (the first argument will be returned instead). The final score
// should not change on subsequent calls where the first argument is not NULL.
WINDOW *deathScreen(WINDOW *w, int finalScore, char name[11]);

// Returns false when done handling input (enter pressed with valid name).
bool handleDeathScreenInput(int input, char name[11]);

// Displays information during game play.
// If w is NULL, a window is created and returned. The returned window should be passed as w on subsequent calls.
WINDOW *gameHeader(WINDOW *w, struct gameState *state, struct levelData *level);

#endif
