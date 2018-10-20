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
#include "effects.h"

#include "levels.h"


void initLevelData(struct levelData * level) {
	level->currLevel=1;
	level->numEnemies=0;
	level->maxNumEnemies=4;
	level->spawnOK=1;
	level->AIlevel=0;
	level->numDisps=0;

}

void procGen(struct gameState * state, struct library * lib, struct levelData * level, WINDOW * window) {
	// level 1 open space
	// level 2 asteroid field
	// level 3 planet surface
	
	// set level
	// move this eventually
	// and I think you want finer granularity
	if (state->score > 15 ) {
		level->currLevel = 15;
		level->AIlevel = 1;	
		level->maxNumEnemies = 6;
	}
	if (state->score > 30 && state->score < 45) {
		level->currLevel = 20;
		level->AIlevel = 0;
		level->maxNumEnemies = 3;
	}
	if (state->score > 45 && state->score < 60) {
		level->currLevel = 25;
		level->AIlevel = 1;
		level->maxNumEnemies = 5;
	}
	if (state->score > 60 && state->score < 75) {
		level->currLevel = 30;
		level->AIlevel = 2;
		level->maxNumEnemies = 2;
	}
	// extended scale
	if (state->score > 75) {
		level->maxNumEnemies += (int)((state->score - 75)/10);
	}
//	level->AIlevel = 1;
//	level->maxNumEnemies =2;
	
	// get new enemies (if needed) and set AI
	spawnEnemies(state, lib, level);

	
	// clean up irrelevant enemies & sprites & spawn new stuff for background
	manageSprites(state, lib, level);
}

void spawnEnemies(struct gameState * state, struct library * lib, struct levelData * level) {
	int enemyType=1;
	if (level->numEnemies < level->maxNumEnemies && level->spawnOK > 0) {
		// make a new enemy
		if (level->currLevel < 20) {
			// here you can vary the enemy type by percentage (when you get more enemies)
		}
		addEnemy(state, lib, enemyType, level->AIlevel);
		//addEffect(6,state->allSprites->numSprites-1,state, lib);
		level->numEnemies++;
		level->spawnOK = -9;   // caution (fix me) this is dependent on REFRESH_RATE
		
	}
	else {
		level->spawnOK += 1;
	}


}

void manageSprites(struct gameState * state, struct library * lib, struct levelData * level) {
	float positionTol = 1.1;
	int i;	for (i=1; i < state->allSprites->numSprites; i++) {
	if (fabs(state->allSprites->spriteArr[i]->xLoc - state->maxX)/state->maxX > positionTol ||
		fabs(state->allSprites->spriteArr[i]->yLoc - state->maxY)/state->maxY > positionTol) {
		if (state->allSprites->spriteArr[i]->type == 1) {
			// just this for now -- do you want to differentiate between enemies and projectiles?
			// or make projectiles effects (no physics?) - noodle this
			level->numEnemies--;				
			delSprite(state, i);
		}
		else if (state->allSprites->spriteArr[i]->type == 3) {
			// need to vary this call by level
			delSprite(state,i);
			if (level->currLevel < 2) {
				genOpenSpaceBG(state, lib);
			}
		}
	}
	else if (state->allSprites->spriteArr[i]->markedForDeath < 0) {
		level->numEnemies--;
		// fix me do this!
		// delEffect(state, i);
		delSprite(state, i);
		level->spawnOK = -9;   // caution this is dependent on REFRESH_RATE
	}
}
}

void addEnemy(struct gameState * state, struct library * lib, int ID, int AIlevel) {
	addSprite(ID, state, lib);
	int xAcc = getRand(25,30);
//	xAcc = 0;	// for debugging
	modSprite(-1, state->maxX, getRand(1, state->maxY), -xAcc*(1e6)/REFRESH_RATE, 0, AIlevel, state);
}

void genOpenSpaceBG(struct gameState * state, struct library * lib) {
	// space backrounds are IDs:2-5		// this will change fix me
		addSprite(getRand(2,5), state, lib);
		modSprite(-1, state->maxX, getRand(1,state->maxY), -getRand(1,5)*(1e6)/REFRESH_RATE, 0, 0, state);	
}

void initOpenSpaceBG(struct gameState * state, struct library * lib) {
	// space backrounds are IDs:2-5
	int i, j;
	for (i=1; i< state->maxX; i++) {
		for (j=1; j< state->maxY; j++) {
			if (getRand(0,100)<1) {
				addSprite(getRand(2,5), state, lib);
				modSprite(-1, i, j, -getRand(1,5)*(1e6)/REFRESH_RATE, 0, 0, state);
			}
		}
	}
}

void freeLevelDisps(struct levelData * level) {
	int j;
	for (j=0; j<level->numDisps; j++) {
		free(level->dispArr[j]);
	}
}

// should i live here?  this is the main place i use pct, and i
// don't have any other reason for an interfaces.c yet
int getRand(int low, int hi) {
	return (rand() % (hi-low+1)) + low;
}
