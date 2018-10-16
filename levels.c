/*******************************************************************************************
** Author: Chris Spravka
** Date:  15 OCT 2018
** Description: levels implementation file

NOTES
1.  this is just the start to get the skeleton rolling
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
//#include "gamePlay.h"
//#include "menu.h"
#include "sprites.h"

#include "levels.h"


void initLevelData(struct levelData * level) {
	level->currLevel=0;
	level->numEnemies=0;
	level->maxNumEnemies=2;
	level->spawnOK=1;
	level->AIlevel=0;
	level->numDisps=0;
}

void procGen(struct gameState * state, struct library * lib, struct levelData * level) {
	// level 1 open space
	// level 2 asteroid field
	// level 3 planet surface
	
	// fix me (clearly)
	if (state->score < 1000 && state->time > 0) {
		// level 1, dumb AI
		//state->allSprites->spriteArr[state->allSprites->numSprites-2]->xAcc += -1*(1e6)/REFRESH_RATE;
		if (level->numEnemies < level->maxNumEnemies && level->spawnOK == 1) {
			// make a new enemy
			// figure out how to delay enemy gen and reset spawnOK
			addSprite(1, state, lib);
			level->numEnemies++;
			int unity;
			if (state->allSprites->numSprites < 3) {
				unity = -1;
			}
			else {
				unity =1;
			}
			state->allSprites->spriteArr[state->allSprites->numSprites-1]->xAcc += unity*(1e6)/REFRESH_RATE;
			level->spawnOK = 1;
		}
	}
	
}

void freeLevelDisps(struct levelData * level) {
	int j;
	for (j=0; j<level->numDisps; j++) {
		free(level->dispArr[j]);
	}
}