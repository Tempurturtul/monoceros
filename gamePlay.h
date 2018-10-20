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

struct absLoc {
	int x[MAX_SPRITE_DISPS*MAX_DISP_SUBSIZE];
	int y[MAX_SPRITE_DISPS*MAX_DISP_SUBSIZE];
	int numChars;
};

void initGame(struct gameState * state, struct library * lib, struct levelData * level);

void updatePhysics(struct gameState * state);

void detectCollision(struct gameState * state, struct library * lib, struct levelData * level );
float calcDistance(struct sprite * s1, struct sprite * s2);
int checkOverlap(struct sprite * s1, struct sprite * s2);
void calcAbsLoc(struct sprite * spriteIn, struct absLoc * loc);
//void playGame(struct gameState * state, struct library * lib, struct levelData * level);
void calcScore(struct gameState * state, struct levelData * level);
void playGame();
void waitQueue();

#endif