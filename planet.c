/*******************************************************************************************
** Author: Chris Spravka
** Date:  01 NOV 2018
** Description: planet implementation file

NOTES
1.  planet is fundamentally different from other levels and has a ton of code, so I moved it here
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>


#include "sprites.h"
#include "effects.h"

#include "planet.h"

void planetLevel(struct gameState * state, struct library * lib, struct levelData * level) {
	int i, j;
	level->spawnOK=0;
	if (level->skyRate<2*level->skyLimit) {
		initPlanetBG(state, lib, level);
	}
	// you want this to be an else if
	else {
		if (level->skyRate == 2*level->skyLimit) {
			level->spawnOK=0;
			for(j=0; j< 8; j++) {
				for (i=0; i < state->maxX+2; i++ ) {
					addSprite(gnd1, state, lib);
					gndSprite(-1, i, state->maxY+40+j, 0,-3, state);	
				}				
			}

			level->skyRate++;
		}
		else {
			for (i=0; i<state->allSprites->numSprites; i++) {
				if (state->allSprites->spriteArr[i]->type == 5 && 
					state->allSprites->spriteArr[i]->yLoc <= state->maxY-7) {
						// stop sprites
						transitionPlanetBG(state, lib, level);
						// modify for cyan sky background
						//wbkgd(window, COLOR_PAIR(15));
						state->skyReady=1;
						swapSkyMotif(state, lib);
						// okay to go
						level->spawnOK=1;
					}
			}
		}
	}

}

void swapSkyMotif(struct gameState * state, struct library * lib) {
	int j;
	// fix player ship colors
	// state->allSprites->spriteArr[0]->dispArr[0]->colorPair = 16;
	state->allSprites->spriteArr[0]->dispArr[0]->colorPair = 17;
	//fix laser colors
	lib->allEffects->effectArr[laserEffect]->dispArr[0]->colorPair = 15;
	lib->allEffects->effectArr[laserEffect]->dispArr[1]->colorPair = 15;
	lib->allEffects->effectArr[laserEffect]->dispArr[2]->colorPair = 16;
	// fix missile colors
	lib->allSprites->spriteArr[missileRt]->dispArr[0]->colorPair = 15;
	lib->allSprites->spriteArr[missileRt]->dispArr[1]->colorPair = 19;
	// fix left missile colors
	lib->allSprites->spriteArr[missileLt]->dispArr[0]->colorPair = 19;
	lib->allSprites->spriteArr[missileLt]->dispArr[1]->colorPair = 15;
	// fix enemy ship explosion
	lib->allEffects->effectArr[shipEx1]->dispArr[0]->colorPair = 15;
	lib->allEffects->effectArr[shipEx1]->dispArr[1]->colorPair = 22;
	lib->allEffects->effectArr[shipEx1]->dispArr[2]->colorPair = 17;
	lib->allEffects->effectArr[shipEx1]->dispArr[3]->colorPair = 17;
	// fix player ship explosion
	for (j=0; j < lib->allEffects->effectArr[shipEx3]->numDisps; j++) {
		lib->allEffects->effectArr[shipEx3]->dispArr[j]->colorPair = 15;
	}
	lib->allEffects->effectArr[shipEx2]->dispArr[0]->colorPair = 17;
	lib->allEffects->effectArr[shipEx2]->dispArr[1]->colorPair = 16;
	lib->allEffects->effectArr[shipEx2]->dispArr[2]->colorPair = 16;
	lib->allEffects->effectArr[shipEx2]->dispArr[3]->colorPair = 14;
	lib->allEffects->effectArr[shipEx2]->dispArr[4]->colorPair = 22;
	lib->allEffects->effectArr[shipEx2]->dispArr[5]->colorPair = 22;	
}

void genPlanetBG(struct gameState * state, struct library * lib, struct levelData * level) {
	// apply gravity
	int i;
	float xExtent=0, yExtent=state->maxY;
	for (i=0; i< state->allSprites->numSprites; i++) {
		struct sprite * temp = state->allSprites->spriteArr[i];
		if (temp->type == 0) {
			temp->yAcc += 0.75*(1e6)/REFRESH_RATE;
		}
		else if (temp->type == 5) {
			// this is looking to find the last column of landscape,
			// along with its height, since you are looping through everyone anyways
			if (temp->xLoc > xExtent) {
				xExtent = temp->xLoc;
				//if (temp->yLoc <= yExtent) {
					yExtent = temp->yLoc;
				//}
			}
			if (temp->xLoc < 0) {
				delSprite(state, i);
			}
			
		}
	}
	
	// builds terrain up and down
	if (getRand(1,100)<20 ) {
		if (getRand(1,100) >50  && state->maxY - yExtent< level->maxHeight) {
			// buids up
			yExtent += -getRand(1,5);
		}
		else if (yExtent < state->maxY) {
			// goes down
			yExtent += +getRand(1,5);
		}
	}
	// actually buid when appropriate, fill in with non-interacting sprites to 
	// save overhead computation
	while (xExtent < state->maxX+2) {
			addSprite(gnd1, state, lib);
			gndSprite(-1,xExtent+1, (int)yExtent, -level->groundVel,0, state);	
		for (i = (int)yExtent+1; i < state->maxY; i++) {
			addSprite(gnd1, state, lib);
			gndSprite(-1,xExtent+1, i, -level->groundVel,0, state);	
			state->allSprites->spriteArr[state->allSprites->numSprites-1]->type=3;

		}
		xExtent++;
	}
	state->gndHeight = yExtent;
	
}
void transitionPlanetBG(struct gameState * state, struct library * lib, struct levelData * level) {
	int i;
	// gosh don't really need two logic checks here anymore
	// clear the sprites that are still hanging out from the previous level
	for (i=0; i<state->allSprites->numSprites; i++) {
		struct sprite * temp = state->allSprites->spriteArr[i];
		if (temp->type == 5 ) {
			gndSprite(i, -1000, -1000, -level->groundVel,0, state);
		}
		else if (temp->type == 3) {
			gndSprite(i, -1000, -1000, -level->groundVel, 0, state);
		}
	}
	level->groundOK = 1;
	
}

void initPlanetBG(struct gameState * state, struct library * lib, struct levelData * level) {
	int i;
	int down;
	down = level->skyRate;
	if (level->skyRate > level->skyLimit) {
		down = level->skyLimit - (level->skyRate - level->skyLimit);
	}
	for (i=0; i< state->allSprites->numSprites; i++) {
		struct sprite * temp = state->allSprites->spriteArr[i];
		// make sprites feel like they are moving up
		if (temp->type == 3 || temp->type == 2) {
			temp->yAcc += -0.5*(1e6)/REFRESH_RATE;
		}
		// make player feel like they are moving down
		else if (temp->type == 0) {
			temp->yAcc += 0.5*(1e6)/REFRESH_RATE;
		}
	}
	// reentry sprites!
	for (i=0; i<state->maxX; i++) {
		if (getRand(1,100) < down) {
			addSprite(getRand(sky1,sky2), state, lib);
			if (level->skyRate > level->skyLimit) {
				modSprite(-1, i, state->maxY, 0,-3.0*(1e6)/REFRESH_RATE, 0, state);
			}
			else {
				modSprite(-1, i, state->maxY, 0,-1.0*(1e6)/REFRESH_RATE, 0, state);			
			}
		}
	}
	level->skyRate++;
	
}
