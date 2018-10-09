/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: menu header file
*******************************************************************************************/
#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <ncurses.h>

int getMenu(WINDOW*, WINDOW*);

void dispScores(WINDOW*, WINDOW*);

int centerText(WINDOW* window, const char * text);
int needInput(WINDOW * window, char * chInput_in);

#endif