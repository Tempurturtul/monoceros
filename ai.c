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

void updateSpriteAI(struct gameState * state, struct library * lib) {
	int i;
	float shootDelay = 3; // seconds
	float shootTol = 5; // pixels
	struct spriteList * list = state->allSprites;
	for (i=1; i< list->numSprites; i++) {
		struct sprite * sIn = list->spriteArr[i];
		if (sIn->type == 1 && sIn->markedForDeath == 0) {
			calcMovement(i, state);
			// do something for shooters
			if (sIn->isShooter > 0 ) {
				struct sprite * pShip = state->allSprites->spriteArr[0];
				if (state->time - sIn->isShooter > shootDelay  &&
						fabs(sIn->yLoc-pShip->yLoc) < shootTol) {
						// fire at will!
						addSprite(missleLt, state, lib);
						// if you want to be real physicsy then you'd actually copy the pShip
						// velocities to the new sprite as well
						modSprite(-1, sIn->xLoc+sIn->xCoM-1, sIn->yLoc+sIn->yCoM, -60*(1e6)/REFRESH_RATE, 0, 0, state);

						sIn->isShooter = state->time;
				}
			}
		}
	}
}

void calcMovement(int sIndex, struct gameState * state) {
	struct sprite * sIn = state->allSprites->spriteArr[sIndex];
	switch (sIn->AI) {
		case 0:
			break;
		case 1: 
			trackP(sIndex, state);
			break;
		case 2:
			trackPI(sIndex, state);
			break;
		default:
			// shouldn't get here
			break;
	}
}

void trackP(int sIndex, struct gameState * state) {
	// define baseThrust in sprite? probably
	float gain=getRand(2,10);
	float baseThrust = 0.1;
	struct sprite * target = state->allSprites->spriteArr[0];
	struct sprite * sIn = state->allSprites->spriteArr[sIndex];
	sIn->yAcc += gain*(target->yLoc/sIn->yLoc-1)*(baseThrust*(1e6)/REFRESH_RATE);
}
void trackPI(int sIndex, struct gameState * state) {
	// proportional and integrator controller
	float gain=0.5;
	float baseThrust = getRand(1,6);
	struct sprite * target = state->allSprites->spriteArr[0];
	struct sprite * sIn = state->allSprites->spriteArr[sIndex];
	float errCur = target->yLoc - sIn->yLoc;
	sIn->yAcc += gain*((5.0*(errCur - sIn->errLast) + errCur)/sIn->yLoc)*(baseThrust*(1e6)/REFRESH_RATE);
	sIn->errLast = errCur;
}
