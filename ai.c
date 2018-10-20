/*******************************************************************************************
** Authors: Chris Spravka
** Date:  18 OCT 2018
** Description: enemy AI implementation file

NOTES
1. intent(? currently?) for AILevels:
	0 - no AI
	1 - move along vertical access to intercept player
	2 - intercept player for specific period

2. how do i incorporate shooting, projectiles? lasers? 
with AI or with specific enemies?
*******************************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include <time.h>
#include <stdio.h>
#include <math.h>

#include "sprites.h"
#include "ai.h"

void updateSpriteAI(struct gameState * state) {
	int i;
	struct spriteList * list = state->allSprites;
	for (i=1; i< list->numSprites; i++) {
		struct sprite * sIn = list->spriteArr[i];
		if (sIn->type == 1 && sIn->markedForDeath == 0) {
			calcMovement(i, state);
			// do something for shooters
		}
	}
}

void calcMovement(int sIndex, struct gameState * state) {
	struct sprite * sIn = state->allSprites->spriteArr[sIndex];
	switch (sIn->AI) {
		case 0:
			break;
		case 1: 
			trackY(sIndex, state);
			break;
		case 2:
			trackXY(sIndex, state);
			break;
		default:
			// shouldn't get here
			break;
	}
}

void trackY(int sIndex, struct gameState * state) {
	// define baseThrust in sprite? probably
	float gain=1.;
	float baseThrust = 1.;
	struct sprite * target = state->allSprites->spriteArr[0];
	struct sprite * sIn = state->allSprites->spriteArr[sIndex];
	sIn->yAcc += gain*(target->yLoc/sIn->yLoc-1)*(baseThrust*(1e6)/REFRESH_RATE);
}
void trackXY(int sIndex, struct gameState * state) {
	// same as above in both x & y
	// but limit time allowed to be active (it'd be impossible, otherwise)
}
