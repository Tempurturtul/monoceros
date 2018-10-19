/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: menu header file
*******************************************************************************************/
#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <ncurses.h>

int mainMenu();
void dispScores(int scores[10], char *names[10]);
void loadingScreen(const char *text, int secondsLoading);
void deathScreen(int score);

#endif