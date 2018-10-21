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

int mainMenu();
void dispScores();
void loadingScreen(const char *text, int secondsLoading);

#endif
