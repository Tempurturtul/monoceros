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
#include <arpa/inet.h>

#include "interfaces.h"

struct absLoc {
	int x[MAX_SPRITE_DISPS*MAX_DISP_SUBSIZE];
	int y[MAX_SPRITE_DISPS*MAX_DISP_SUBSIZE];
	int numChars;
};

void initGame(struct gameState * state, struct library * lib, struct levelData * level);

void updatePhysics(struct gameState * state);
void limitVel(struct sprite * temp, float limit);
void detectCollision(struct gameState * state, struct library * lib);
void manageCollision(int i, int j,struct gameState * state, struct library * lib);
float calcDistance(struct sprite * s1, struct sprite * s2);
int checkOverlap(struct sprite * s1, struct sprite * s2);
void calcAbsLoc(struct sprite * spriteIn, struct absLoc * loc);
//void playGame(struct gameState * state, struct library * lib, struct levelData * level);
void calcScore(struct gameState * state, struct levelData * level);
void playGame(int network_socket);
void waitQueue();
void handleInput(int inputChar, int *playFlag, struct gameState *state, struct library * lib);
void restrictPlaySpace(struct gameState *state);

#endif
