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
	level->currLevel=10;
	level->numEnemies=0;
	level->maxNumEnemies=4;
	level->spawnOK=1;
	level->AIlevel=0;
	level->numDisps=0;
	level->skyRate=-35;
	level->skyLimit=25;
	level->groundVel=5;

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
		level->currLevel = 35;
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
	//int enemyType=1;
	if (level->numEnemies < level->maxNumEnemies && level->spawnOK > 0) {
		// make a new enemy
		if (level->currLevel < 20) {
			// here you can vary the enemy type by percentage (when you get more enemies)
		}
		addEnemy(state, lib, eny1, level->AIlevel);
		if (level->AIlevel>0) {
			// if you want to do this, you'll need a way of activating the effects
			// at the appropriate time (i.e. the AI fires thrusters)
			// addEffect(upThrust1, state->allSprites->numSprites-1,state, lib);
		}
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
					//initPlanetBG(state, lib, level);
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
	if (level->currLevel > 25) {
		if (level->skyRate<2*level->skyLimit) {
			initPlanetBG(state, lib, level);
		}
		else {
			genPlanetBG(state, lib, level);
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
	// space backrounds are openSpace1-4 
	
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

void genPlanetBG(struct gameState * state, struct library * lib, struct levelData * level) {
	// apply gravity
	int noSprite= 0;
	int something = state->maxY-5;
	int i;
	for (i=0; i< state->allSprites->numSprites; i++) {
		struct sprite * temp = state->allSprites->spriteArr[i];
		if (temp->type == 3) {
			temp->yAcc += -0.5*(1e6)/REFRESH_RATE;
		}
		else if (temp->type == 0) {
			temp->yAcc += 1.0*(1e6)/REFRESH_RATE;
		}
		// this here?!
		if (temp->xLoc == state->maxX && temp->yLoc == something) {
			noSprite = 1;
		}
	}
	// need to modify baddies so they are only coming from 'above ground'
	// do you want the ground to be destructable?
	if (!noSprite) {
		addSprite(gnd1, state, lib);
		gndSprite(-1, state->maxX, something, -5, 0, state);	
		noSprite=0;
		
	}
	
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
		if (temp->type == 3) {
			temp->yAcc += -0.5*(1e6)/REFRESH_RATE;
		}
		else if (temp->type == 0) {
			temp->yAcc += 0.5*(1e6)/REFRESH_RATE;
		}
	}
	for (i=0; i<state->maxX; i++) {
		if (getRand(1,100) < down) {
			addSprite(getRand(sky1,sky2), state, lib);
			modSprite(-1, i, state->maxY, 0,-1.0*(1e6)/REFRESH_RATE, 0, state);			
		}
	}
	level->skyRate++;
	
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
