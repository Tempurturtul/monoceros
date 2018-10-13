/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: menu header file
*******************************************************************************************/
#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <ncurses.h>

int mainMenu();
void dispScores();
void loadingScreen(int startUnixtime);

#endif