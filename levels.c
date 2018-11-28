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
#include "planet.h"


void initLevelData(struct levelData * level) {
	level->currLevel=10;
	level->numEnemies=0;
	level->maxNumEnemies=4;
	level->spawnOK=1;
	level->AIlevel=0;
//	level->numDisps=0;
	level->skyRate=-35;
	level->skyLimit=20;
	level->groundVel=15;
	
	level->groundOK=0;
	level->maxHeight=25;
	
	level->pctAmmo=10;

}


void setMaxHeight(struct levelData * level, struct gameState * state) {
	// the -7 is just to ensure that the player will always have some amount of space toascii
	// fly through
	level->maxHeight = state->maxY - state->titleSize - 7; 
}
void procGen(struct gameState * state, struct library * lib, struct levelData * level) {
	// level 1 open space
	// level 2 asteroid field
	// level 3 planet surface
	
	// set level
	// move this eventually
	// and I think you want finer granularity
	if (state->score > 15 && state->score < 30) {
		level->currLevel = 15;
		level->AIlevel = 1;	
		level->maxNumEnemies = 6;
	}
	if (state->score > 30 && state->score < 55) {
		level->currLevel = 20;
		level->AIlevel = 0;
		level->maxNumEnemies = 3;
	}
	if (state->score > 55 && state->score < LEVEL_THREE_SCORE) {
		level->currLevel = 25;
		level->AIlevel = 1;
		level->maxNumEnemies = 5;
	}
	if (state->score > LEVEL_THREE_SCORE && state->score < EXTENDED_SCORE) {
		level->currLevel = 30;
		level->AIlevel = 2;
		level->maxNumEnemies = 2;
	}
	// extended scale
	if (state->score > EXTENDED_SCORE) {
		level->currLevel = 35;
		level->maxNumEnemies += (int)((state->score - 75)/10);
		//level->groundVel++;
	}
	// debugging ai
	/*
	level->AIlevel = 1;
	level->maxNumEnemies =2;
	if (level->numEnemies >= 1) {
		level->AIlevel=2;
	}
	*/
	
	// get new enemies (if needed) and set AI
	spawnEnemies(state, lib, level);

	
	// clean up irrelevant enemies & sprites & spawn new stuff for background
	manageSprites(state, lib, level);
}

void spawnEnemies(struct gameState * state, struct library * lib, struct levelData * level) {
	int enemyType, AIused;
	if (level->numEnemies < level->maxNumEnemies && level->spawnOK > 0) {
		// make a new enemy - vary by level and type stochasitcally
		if (level->AIlevel == 2) {
			enemyType = eny2;
			AIused = 2;
		}
		else{
			if (getRand(1,100) < 40 ) { 
				AIused = level->AIlevel;
			}
			else {
				AIused = 0;
			}
			enemyType = eny1;
			if (AIused == 1) {
				enemyType = eny1a;
			}
		}
		addEnemy(state, lib, enemyType, AIused);
		state->allSprites->spriteArr[state->allSprites->numSprites-1]->isShooter =1;
	
		if (level->AIlevel>0) {
			// if you want to do this, you'll need a way of activating the effects
			// at the appropriate time (i.e. the AI fires thrusters)
			// addEffect(upThrust1, state->allSprites->numSprites-1,state, lib);
		}
		level->numEnemies++;
		level->spawnOK = -9;   // caution (fix me) this is dependent on REFRESH_RATE
		
	}
	else {
		level->spawnOK += 1;
		// goodies!
		// careful this ends up being REFRESH_RATE dependent
		if (getRand(1,1000) < level->pctAmmo) {
			if (getRand(1,100) < 50 ) {
				addEnemy(state, lib, ammoPC, 0);
			}
			else {
				addEnemy(state, lib, ammoM, 0);
			}
		}
	}
}

void manageSprites(struct gameState * state, struct library * lib, struct levelData * level) {
	float positionTol = 1.1;
	int i;	
	for (i=1; i < state->allSprites->numSprites; i++) {
		struct sprite * temp = state->allSprites->spriteArr[i];
		if (fabs(temp->xLoc - state->maxX)/state->maxX > positionTol ||
			fabs(temp->yLoc - state->maxY)/state->maxY > positionTol) {
			if (temp->type == 1) {
				// just this for now -- do you want to differentiate between enemies and projectiles?
				// or make projectiles effects (no physics?) - noodle this
				level->numEnemies--;				
				delSprite(state, i);
			}
			else if (temp->type == 3 || temp->type == 4) {
				// need to vary this call by level
				delSprite(state,i);
				if (level->currLevel < 20) {
					genOpenSpaceBG(state, lib);
				}
				else if (level->currLevel <30) {
					genAsteroidBG(state, lib, level);
				}
				else {
					// do nothing at level 3!
					// because there's so much going on do this separately
				}
			}

		}
		else if (temp->markedForDeath < 0) {
			if (temp->type == 1) {
				level->numEnemies--;
			}
			else if (temp->type == 4) {
				genAsteroidBG(state, lib, level);
			}
			// fix me do this!
			// delEffect(state, i);
			delSprite(state, i);
			level->spawnOK = -9;   // caution this is dependent on REFRESH_RATE
		}
	}
	if (level->currLevel > 25  && !level->groundOK) {
		planetLevel(state, lib, level);
	}
	else if (level->groundOK) {
		genPlanetBG(state, lib, level);
	}	

}



void addEnemy(struct gameState * state, struct library * lib, int ID, int AIlevel) {
	addSprite(ID, state, lib);
	int xAcc = getRand(20,45);
//	xAcc = 0;	// for debugging
	modSprite(-1, state->maxX, getRand(1, state->gndHeight), -xAcc*(1e6)/REFRESH_RATE, 0, AIlevel, state);
}

void genOpenSpaceBG(struct gameState * state, struct library * lib) {
	// space backrounds!
		addSprite(getRand(openSpace1, openSpace4), state, lib);
		modSprite(-1, state->maxX, getRand(1,state->maxY), -getRand(1,5)*(1e6)/REFRESH_RATE, 0, 0, state);	
}

void initOpenSpaceBG(struct gameState * state, struct library * lib) {
	// space backrounds are openSpace1-4  (non-interacting background)
	int i, j;
	for (i=1; i< state->maxX; i++) {
		for (j=1; j< state->maxY; j++) {
			if (getRand(0,100)<1) {
				addSprite(getRand(openSpace1, openSpace4), state, lib);
				modSprite(-1, i, j, -getRand(1,5)*(1e6)/REFRESH_RATE, 0, 0, state);
			}
		}
	}
}

void genAsteroidBG(struct gameState * state, struct library * lib, struct levelData * level) {
	// asteroids are asteroid1 & 2	
	float yVal, yAcc, xVal, xAcc;
	// just off screen
	if (getRand(1,100) < 50) {
		yVal = state->maxY+5;
		yAcc = -1;
	}
	else {
		yVal = -5;
		yAcc = 1;
	}
	if (getRand(1,100) <25) {
		xAcc=1;
		xVal = getRand(0, state->maxX/3)-5;
	}
	else {
		xAcc=-1;
		xVal = getRand(state->maxX/3, state->maxX)+5;
	}
	if (level->spawnOK > 0) {
		addSprite(getRand(asteroid1, asteroid3), state, lib);
		modSprite(-1, xVal, yVal, xAcc*getRand(5,20)*(1e6)/REFRESH_RATE, yAcc*getRand(2,10)*(1e6)/REFRESH_RATE, 0, state);	
		level->spawnOK = -2;
	}
	else {
		level->spawnOK +=2;
	}
}

// ended up not needing this based on my implementation of the levels
/*
void freeLevelDisps(struct levelData * level) {
	int j;
	for (j=0; j<level->numDisps; j++) {
		free(level->dispArr[j]);
	}
}
*/

// should i live here?  this is the main place i use pct, and i
// don't have any other reason for an interfaces.c yet
int getRand(int low, int hi) {
	return (rand() % (hi-low+1)) + low;
}

void limiter(float * valIn, float low, float high) {
	if (*valIn > high) {
		*valIn = high;
	}
	else if (*valIn < low) {
		*valIn = low;
	}
}