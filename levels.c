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
	level->skyLimit=20;
	level->groundVel=15;
	
	level->groundOK=0;
	level->maxHeight=25;

}

void procGen(struct gameState * state, struct library * lib, struct levelData * level, WINDOW * window) {
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
	manageSprites(state, lib, level, window);
}

void spawnEnemies(struct gameState * state, struct library * lib, struct levelData * level) {
	//int enemyType=1;
	if (level->numEnemies < level->maxNumEnemies && level->spawnOK > 0) {
		// make a new enemy
		if (level->currLevel < 20) {
			// here you can vary the enemy type by percentage (when you get more enemies)
		}
		// debugging!!!!
	if (level->AIlevel == 2) {
		addEnemy(state, lib, eny2, level->AIlevel);
	}
	else{
		addEnemy(state, lib, eny1, level->AIlevel);
		state->allSprites->spriteArr[state->allSprites->numSprites-1]->isShooter =1;
	}
	
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

void manageSprites(struct gameState * state, struct library * lib, struct levelData * level, WINDOW * window) {
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
	i=0;
	int j;
	if (level->currLevel > 25  && !level->groundOK) {
		if (level->skyRate<2*level->skyLimit) {
			initPlanetBG(state, lib, level);
		}
		// you want this to be an else if
		else {
			if (level->skyRate == 2*level->skyLimit) {
				//wbkgd(window, COLOR_PAIR(13));
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
						}
				}
			}
			//genPlanetBG(state, lib, level);
		}
		
	}
	if (level->groundOK) {
		genPlanetBG(state, lib, level);
	}
}


void addEnemy(struct gameState * state, struct library * lib, int ID, int AIlevel) {
	addSprite(ID, state, lib);
	int xAcc = getRand(25,30);
//	xAcc = 0;	// for debugging
	modSprite(-1, state->maxX, getRand(1, state->gndHeight), -xAcc*(1e6)/REFRESH_RATE, 0, AIlevel, state);
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
	// need to modify baddies so they are only coming from 'above ground'
	// do you want the ground to be destructable?
	if (getRand(1,100)<20 ) {
		if (getRand(1,100) >50  && state->maxY - yExtent< level->maxHeight) {
			// buids up
			yExtent += -getRand(1,5);
		}
		else if (yExtent < state->maxY) {
			// goes down
			yExtent += +getRand(1,5);
		}
		// limit yExtent 
		//limiter(&yExtent,state->maxY- level->maxHeight, state->maxY);
	}
	while (xExtent < state->maxX+2) {
			addSprite(gnd1, state, lib);
			gndSprite(-1,xExtent+1, (int)yExtent, -level->groundVel,0, state);	
		for (i = (int)yExtent+1; i < state->maxY; i++) {
			addSprite(gnd1, state, lib);
//			gndSprite(-1, state->maxX-20, 50, -level->groundVel,0, state);		
//			gndSprite(-1,xExtent+1, (int)yExtent, -level->groundVel,0, state);	
			gndSprite(-1,xExtent+1, i, -level->groundVel,0, state);	
			state->allSprites->spriteArr[state->allSprites->numSprites-1]->type=3;

//	printf("x:%f y:%i\n",xExtent,i);
//	sleep(1);
		}
		xExtent++;
	}
	state->gndHeight = yExtent;
	
}
void transitionPlanetBG(struct gameState * state, struct library * lib, struct levelData * level) {
	int i;
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
		if (temp->type == 3 || temp->type == 2) {
			temp->yAcc += -0.5*(1e6)/REFRESH_RATE;
		}
		else if (temp->type == 0) {
			temp->yAcc += 0.5*(1e6)/REFRESH_RATE;
		}
	}
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

void limiter(float * valIn, float low, float high) {
	if (*valIn > high) {
		*valIn = high;
	}
	else if (*valIn < low) {
		*valIn = low;
	}
}