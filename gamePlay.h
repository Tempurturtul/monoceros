/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: gameplay header file
*******************************************************************************************/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "interfaces.h"



void initGame(struct gameState * state, struct library * lib, struct levelData * level);

void updatePhysics(struct gameState * state);

//void playGame(struct gameState * state, struct library * lib, struct levelData * level);
void playGame();
void waitQueue();

#endif