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

// Displays the player's final score and populates `nameBuffer` with the player's input for their name (max of 10 characters).
void deathScreen(int score, char nameBuffer[11]);

#endif
