/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: gamePlay implementation file

NOTES
1. currently just a rough working sketch, need to clean up and move things around
2. need to determine game state structure and what needs to be passed to clients
*******************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <time.h>
#include <math.h>

#include "gamePlay.h"
#include "menu.h"
#include "sprites.h"
#include "effects.h"
#include "levels.h"
#include "ai.h"
#include "tcp_client.h"
#include "scores.h"

void initGame(struct gameState * state, struct library * lib, struct levelData * level) {
	// init your library
	lib->allSprites = malloc(sizeof(struct spriteList));
	initSpriteLibrary(lib->allSprites);
	lib->allEffects = malloc(sizeof(struct effectList));
	initEffectLibrary(lib->allEffects);

	// init state sprites
	state->allSprites = malloc(sizeof(struct spriteList));
	state->allSprites->numSprites = 0;
	addSprite(0, state, lib);
	state->allSprites->spriteArr[0]->isShooter = 0;

	// init state effects
	state->allEffects = malloc(sizeof(struct effectList));
	state->allEffects->numEffects = 0;
	// these are the effects for the player ship thrusters
	addEffect(0, 0, state, lib);
	addEffect(1, 0, state, lib);
	addEffect(2, 0, state, lib);
	addEffect(3, 0, state, lib);
	addEffect(4, 0, state, lib);
	addEffect(5, 0, state, lib);
	addEffect(6, 0, state, lib);

	// init the level
	initLevelData(level);

	// init state variables
	state->deltaKills = 0;
	state->timeWait = -REFRESH_RATE/1e6;
	state->timeLast=-REFRESH_RATE/1e6;
	state->time = 0;
	state->score = 0; //55;  // debugging! gets you to planet level (stage 3)
	state->scoreTimeLast =0;
	state->maxX=1;
	state->maxY=1;
	state->titleSize = 3;
	state->playFlag=1;
	state->skyReady=0;
	
	usleep(REFRESH_RATE);
}

void freeGame(struct gameState * state, struct library * lib, struct levelData * level) {
	freeSpriteList(state->allSprites);
	freeEffectList(state->allEffects);

	freeSpriteList(lib->allSprites);
	freeEffectList(lib->allEffects);

//	freeLevelDisps(level);

	free(state);
	free(lib);
	free(level);

}
void playGame(int network_socket) {
	// now you're the clients
	int inputChar;
	int vertCtrl=-1;
	// determine window size to let the server adjudicate
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	
	// make space for state, level and lib?
	struct library * lib = malloc(sizeof(struct library));
	struct gameState * state = malloc(sizeof(struct gameState));
	struct levelData * level = malloc(sizeof(struct levelData));
	// using for memory purposes only - don't really need
	initGame(state, lib, level);

	// send screen size to server
	//printf("x1,y1:%i,%i\n", maxX, maxY); sleep(2);
	send_data(network_socket, &maxX, sizeof(int));
	send_data(network_socket, &maxY, sizeof(int));
	
	// get the state back!
//	printf("init sprites? num: %i\n", state->allSprites->numSprites);
//	printf("float(%lu), int(%lu), dispArrAddr(%lu), sprite(%lu), char(%lu), dispPair(%lu)\n", sizeof(float), sizeof(int), sizeof(struct dispPair *), sizeof(struct sprite), sizeof(char), sizeof(struct dispPair));
	receive_data(network_socket, state, lib, level);
	
	recv(network_socket, &vertCtrl, sizeof(int), 0);
	
	WINDOW *title = newwin(state->titleSize, state->maxX, 0, 0);
	WINDOW *action = newwin(state->maxY-state->titleSize, state->maxX, state->titleSize, 0);
	
// 	no long need this razzmatazz with how you are now handling wbkgd()
//	WINDOW *title = newwin(state->titleSize, maxX, 0, 0);
//	WINDOW *action = newwin(maxY-state->titleSize, maxX, state->titleSize, 0);
	// assume this is the largest window - if it's not your're allready covered and this will ERR
//	WINDOW *dum1 = newwin(maxY,maxX,0,state->maxX);
//	WINDOW *dum2 = newwin(maxY,maxX,state->maxY,0);

	//createDummyWindows(state,maxX, maxY, dum1, dum2);

	// Capture arrow key input.
	keypad(action, TRUE);
	// sets the blocking timer for wgetch
	wtimeout(action, 1);	
	
	// timing (wall clock)?
	
	// main play loop, will modify flag on player death or quit
	while (state->playFlag) {
	
		// get input
		inputChar = wgetch(action);
		scrubInput(vertCtrl, &inputChar);
		
		// send input
		send_data(network_socket, &inputChar, sizeof(int));
		
		// get results (state, level, lib)
		receive_data(network_socket, state, lib, level);

		// display results
		// this all could be done elsewhere i think, make a separate func
		wclear(title);
		wclear(action);
		wcolor_set(action, 1, NULL);		// change this by referencing the appropriate colors in the sprites and effects themselves
		
/*		// see above
		wclear(dum1);
		wclear(dum2);	
		wcolor_set(dum1, 1, NULL);
		wcolor_set(dum2, 1, NULL);
*/
		// prints all sprites
		printSprite(action, state);		

		// prints all effects, very similar functions
		printEffect(action, state);
		
		// i needed this for debugging
		mvwprintw(title, 0, 1, "xLoc:%f",state->allSprites->spriteArr[0]->xLoc);
		mvwprintw(title, 1, 1, "xVel:%f",state->allSprites->spriteArr[0]->xVel);
		mvwprintw(title, 2, 1, "xMax:%i",state->maxX);

		mvwprintw(title, 0, 20, "numEnemies:%i",level->numEnemies);
		mvwprintw(title, 1, 20, "numSprites:%i",state->allSprites->numSprites);
		//mvwprintw(title, 2, 20, "yVel:%f",state->allSprites->spriteArr[0]->xAcc);

		mvwprintw(title, 0, 40, "numEffects:%i",state->allEffects->numEffects);
//		mvwprintw(title, 1, 40, "radius gnd:%f",lib->allSprites->spriteArr[gnd1]->radius);
//		mvwprintw(title, 2, 40, "xCoM gnd:%f",lib->allSprites->spriteArr[gnd1]->xCoM);
		//mvwprintw(title, 2, 40, "numDisps eff6:%i",lib->allEffects->effectArr[6]->numDisps);

		mvwprintw(title, 1, 60, "AMMO:%i",(int)state->allSprites->spriteArr[0]->isShooter);

		
		mvwprintw(title, 0, state->maxX - 15, "time: %.1f", round(state->time*10)/10);
		mvwprintw(title, 1, state->maxX - 15, "SCORE: %i", state->score);
		mvwprintw(title, 2, state->maxX - 15, "LEVEL: %i", level->currLevel);
		
		// actually print!
		wrefresh(title);
		wrefresh(action);
		// see above
//		wrefresh(dum1);
//		wrefresh(dum2);	
		
		// timing - no? handle at server - client will just 'keep up'
		
	// end main
	}
	
	// free space for state, level, lib
	freeGame(state, lib, level);	// FIXME - need to do this

	delwin(title);
	delwin(action);	
}

//void playGame(struct gameState * state, struct library * lib, struct levelData * level) {
//void playGameSingle(int network_socket) {
void playGameSingle() {
	// Create windows.
	int maxX, maxY;
	int titleSize = 3;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *title = newwin(titleSize, maxX, 0, 0);
	WINDOW *action = newwin(maxY-titleSize, maxX, titleSize, 0);


	// Capture arrow key input.
	keypad(action, TRUE);

	struct library * lib = malloc(sizeof(struct library));
	struct gameState * state = malloc(sizeof(struct gameState));
	struct levelData * level = malloc(sizeof(struct levelData));

	initGame(state, lib, level);
	state->maxX = maxX;
	state->maxY = maxY-titleSize;
	state->gndHeight = maxY;

	// init background (only needed for level 1, will fly into other backgrounds)
	initOpenSpaceBG(state, lib);
	
	int inputChar;

	// sets the blocking timer for wgetch
	wtimeout(action, 1);

	// init timing function
	struct timespec timeHold;
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	float tstart = timeHold.tv_sec + timeHold.tv_nsec / 1e9;

	// main play loop, will modify flag on player death or quit
	 while (state->playFlag) {
		//tstart = clock();

		inputChar = wgetch(action);

		handleInput(inputChar, &(state->playFlag), state, lib);

		restrictPlaySpace(state);

		// procedural level generation
		procGen(state, lib, level);

		// this updates the sprite actions (including firing)
		updateSpriteAI(state, lib);
		// update physics for all sprites
		updatePhysics(state);

		// handles collision detection and collision results
		detectCollision(state, lib);

		// calculates the current score, stores it in state
		calcScore(state, level);

		// draw screen!

		// this all could be done elsewhere i think, make a separate func
		wclear(title);
		wclear(action);
		wcolor_set(action, 1, NULL);		// change this by referencing the appropriate colors in the sprites and effects themselves
		
		// prints all sprites
		printSprite(action, state);
		// prints all effects, very similar functions
		printEffect(action, state);
		
		// i needed this for debugging
		mvwprintw(title, 0, 1, "xLoc:%f",state->allSprites->spriteArr[0]->xLoc);
		mvwprintw(title, 1, 1, "xVel:%f",state->allSprites->spriteArr[0]->xVel);
		mvwprintw(title, 2, 1, "xMax:%i",state->maxX);

		mvwprintw(title, 0, 20, "numEnemies:%i",level->numEnemies);
		mvwprintw(title, 1, 20, "numSprites:%i",state->allSprites->numSprites);
		//mvwprintw(title, 2, 20, "yVel:%f",state->allSprites->spriteArr[0]->xAcc);

		mvwprintw(title, 0, 40, "numEffects:%i",state->allEffects->numEffects);
		mvwprintw(title, 1, 40, "radius gnd:%f",lib->allSprites->spriteArr[gnd1]->radius);
		mvwprintw(title, 2, 40, "xCoM gnd:%f",lib->allSprites->spriteArr[gnd1]->xCoM);
		//mvwprintw(title, 2, 40, "numDisps eff6:%i",lib->allEffects->effectArr[6]->numDisps);

		mvwprintw(title, 1, 60, "AMMO:%i",(int)state->allSprites->spriteArr[0]->isShooter);

		
		mvwprintw(title, 0, maxX - 15, "time: %.1f", round(state->time*10)/10);
		mvwprintw(title, 1, maxX - 15, "SCORE: %i", state->score);
		mvwprintw(title, 2, maxX - 15, "LEVEL: %i", level->currLevel);
		// actually print!
		wrefresh(title);
		wrefresh(action);

		// timing for gameplay, could make this a f(sprites) for
		// smoother behavior - nvmd: normalizing to frames per second is pretty effective
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->timeWait = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;
		if (((1./12)-(state->timeWait-state->time))*1e6 > 0) {
			usleep(((1./12)-(state->timeWait-state->time))*1e6);
		}
		else {
			// do nothing! you're trying to catch up on frame rate!
		}
		// generate the output game time (and time used for physics)
		state->timeLast=state->time;
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->time = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;

		}

	struct highscore newHighscore;
	newHighscore.score = state->score;
	deathScreen(newHighscore.score, newHighscore.name);
	putScore(newHighscore);

	// clean up - do a better job of this!
	freeGame(state, lib, level);

	delwin(title);
	delwin(action);
}


void detectCollision(struct gameState * state, struct library * lib) {
	int i, j;
	float dist;
	struct sprite * s1;
	struct sprite * s2;
	// only check my ship right now
	for (i=0; i<state->allSprites->numSprites; i++) {
		// only check everyone else if current sprite is a colliding type sprite
		s1 = state->allSprites->spriteArr[i];
		if (s1->type != 3 && s1->markedForDeath == 0) {
			// cycle through everyone else
			for (j=0; j< state->allSprites->numSprites; j++) {
				// check for colliding-type and yourself
				// recheck s1 because multiple pixels can be colliding at once (and you don't
				// want to bother with that)
				s2 = state->allSprites->spriteArr[j];
				if (s2->type != 3 && i != j && s2->markedForDeath == 0 && s1->markedForDeath == 0 && s2->type != s1->type) {
					dist = calcDistance(s1,s2);
					// if the distance between the sprites is within one of their spheres of influence
					// only then do a closer check for collision (expensive) (really only need the smaller sphere of influence, but this is safer)
					if (dist < s1->radius ||
						dist < s2->radius) {
						// they are within each other's sphere of influence
						// do a detailed check
						if (checkOverlap(s1, s2)) {
							// determine outcome of collision
							manageCollision(i, j, state, lib);
						}

					}
				}
			}
		}
	}
}

void manageCollision(int i, int j,struct gameState * state, struct library * lib) {
	struct sprite * s1 = state->allSprites->spriteArr[i];
	struct sprite * s2 = state->allSprites->spriteArr[j];
	// 4 is indestructible, so don't bother 
	if (s1->type != 4 ) {
		// 0 is the player, this is checking first if you died (different exposion sequence)
		// then if you got a powerup (cannon or missiles)
		if (i==0) {
			if (s2->type < 6) {
				// you actually hit something to kill you
				addEffect(shipEx2,i,state, lib);
				modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
				addEffect(shipEx3,i,state, lib);
				modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
				s1->markedForDeath=1;
			}
			// powerups!
			else {
				if (s2->type == 6) {
					// switch to missile or add ammo
					s1->isShooter = 25;
					s1->wpnSelect = -1;
					s2->markedForDeath=1;
					addEffect(ammo1,j,state, lib);
					modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
				}
				else if (s2->type == 7) {
					// switch to plasma cannon or add ammo
					s1->isShooter = 15;
					s1->wpnSelect = 1;
					s2->markedForDeath=1;
					addEffect(ammo1,j,state, lib);
					modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
				}
			}
		}
		// not the player, then kill it like normal
		else {
			addEffect(shipEx1,i,state, lib);
			modEffect(-1, state->time, s1->xCoM, s1->yCoM, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
			s1->markedForDeath=1;
		}
		
	}
	// just checking the other guy, 6 & 7 were handled above
	if (s2->type != 4 && s2->type != 6 && s2->type != 7) {
//	if (s2->type < 4) {
		addEffect(shipEx1,j,state, lib);
		modEffect(-1, state->time, s2->xCoM, s2->yCoM, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
		s2->markedForDeath=1;
	}
	// give the player credit if they killed a bad guy
	if (s1->type == 1 || s2->type ==1) {
		state->deltaKills++;
	}	
}
/// ***********//
// should probably move physics to its own header and implementation file

void updatePhysics(struct gameState * state) {
	float dt = state->time - state->timeLast;
	struct spriteList *local = state->allSprites;
	int i=0;
	for (i=0; i< local->numSprites; i++) {
		
		local->spriteArr[i]->yVel += (local->spriteArr[i]->yAcc)*dt;
		if (i==0) {
			limitVel(local->spriteArr[i], 30);
		}

		local->spriteArr[i]->yLoc += ((local->spriteArr[i]->yVel)*dt+0.5*(local->spriteArr[i]->yAcc)*(dt*dt));
		local->spriteArr[i]->yAcc = 0;

		//local->spriteArr[i]->xAcc += 10;		// 10 is my trial grav constant
		local->spriteArr[i]->xVel += (local->spriteArr[i]->xAcc)*dt;
		if (i==0) {
			limitVel(local->spriteArr[i], 30);
		}
		local->spriteArr[i]->xLoc += (((local->spriteArr[i]->xVel)*dt+0.5*(local->spriteArr[i]->xAcc)*(dt*dt)));
		local->spriteArr[i]->xAcc = 0;

		}
}

// oh so pythagoras
float calcDistance(struct sprite * s1, struct sprite * s2) {
	float x1, x2, y1, y2;
	x1 = s1->xLoc+s1->xCoM;
	y1 = s1->yLoc+s1->yCoM;
	x2 = s2->xLoc+s2->xCoM;
	y2 = s2->yLoc+s2->yCoM;

	return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)  );

}

int checkOverlap(struct sprite * s1, struct sprite * s2) {
	struct absLoc s1Loc;
	struct absLoc s2Loc;

	calcAbsLoc(s1, &s1Loc);
	calcAbsLoc(s2, &s2Loc);
	// run through comparing the absolute locations of printing characters
	// to determine the actual collision
	int i,j;
	for (i=0; i<s1Loc.numChars; i++) {
		for (j=0; j<s2Loc.numChars; j++) {
			if (s1Loc.x[i] == s2Loc.x[j] && s1Loc.y[i] == s2Loc.y[j]) {
				return 1;
			}
		}
	}

	return 0;
}

void calcAbsLoc(struct sprite * spriteIn, struct absLoc * loc) {
	// clear out the struct, just to be safe?
	int init = -1;
	memcpy(loc, &init, sizeof(struct absLoc));
	loc->numChars=0;

	int dispCount;
	char tdisp[MAX_DISP_SUBSIZE];
	char temp[MAX_DISP_SUBSIZE];
	// this is really the same logic from printSprite, but without the printing
	// you really should be making a sep function for this (and printEffect)
	for (dispCount=0; dispCount< spriteIn->numDisps; dispCount++) {
		int i=0, j=0, n=0;
		strcpy(tdisp, spriteIn->dispArr[dispCount]->disp);
		for (i=0; i<= strlen(tdisp); i++) {
			if (tdisp[i] == '\n' || tdisp[i] == '\0') {
				memcpy(temp, &(tdisp[n]), (i-n)*sizeof(char));
				// manually terminate your new string
				temp[i-n] = '\0';
				int dx = 0;
				while (dx < strlen(temp)) {
					if (temp[dx] != ' ') {
						loc->x[loc->numChars] = (int)spriteIn->xLoc+dx;
						loc->y[loc->numChars] = (int)spriteIn->yLoc+j;
						loc->numChars++;
					}
					dx++;
				}
				n=i+1;
				j++;
			}
		}
	}
}
// **** end physics ****//
void calcScore(struct gameState * state, struct levelData * level) {
	// i really don't want a sep score structure
	int ppL1Kill = 2;
	int ppL2Kill = 4;
	int ppL3Kill = 8;
	if (state->time > state->scoreTimeLast+1) {
		// one point for every second of life
		state->score++;
		state->scoreTimeLast = state->time;
	}
	if (state->deltaKills > 0) {
		// how do you want to do this?
		// not a switch b/c i'm nore sure if this is the only
		// combination I want, or if I plan to use decimals for sublevels
		if (level->currLevel < 20) {
			state->score += (ppL1Kill*state->deltaKills);
		}
		else if (level->currLevel < 30) {
			state->score += (ppL2Kill*state->deltaKills);
		}
		else  {
			state->score += (ppL3Kill*state->deltaKills);
		}
		state->deltaKills = 0;
	}
}

void waitQueue() {
	messageScreen("Now connecting...");

//	int network_socket = establish_connection("128.193.36.41", 2997);
//	int network_socket = establish_connection("192.168.0.18", 2997);
	int network_socket = establish_connection(SERVER_IP_ADDR, SERVER_IP_PORT);

	if (network_socket == -1) {
		messageScreen("There was an error connecting to the server");
		getch();
		return;
	}

	messageScreen("Starting game in 3...");
	sleep(1);
	messageScreen("Starting game in 2...");
	sleep(1);
	messageScreen("Starting game in 1...");
	sleep(1);

	playGame(network_socket);

	closing_connection(network_socket);
	return;
}

void limitVel(struct sprite * temp, float limit) {
	if (temp->yVel > 0 && temp->yVel > limit) {
		temp->yVel = limit;
	}
	else if (temp->yVel < 0 && temp->yVel < -limit) {
		temp->yVel = -limit;
	}
	if (temp->xVel > 0 && temp->xVel > limit) {
		temp->xVel = limit;
	}
	else if (temp->xVel < 0 && temp->xVel < -limit) {
		temp->xVel = -limit;
	}
}

void handleInput(int inputChar, int *playFlag, struct gameState *state, struct library * lib) {
	struct sprite * pShip = state->allSprites->spriteArr[0];
	float baseThrust = 8.0;

	if (inputChar == 'q') {
		*playFlag = 0;
	}
	// FOR PLAYER CONTROLS - consider:
						// limiting total velocity -- Done
						// increasing deltaAcc for counter thrust  -- NA (limited total velocity instead)
						// applying counter acc at window borders -- Done
						// limiting limits at window borders -- Done
	else if (inputChar == KEY_UP) {
		//allSprites.spriteArr[0]->yLoc += -1;
		pShip->yAcc += -baseThrust*(1e6)/REFRESH_RATE;
		if (state->score < LEVEL_THREE_SCORE) {
			state->allEffects->effectArr[2]->start = state->time;
			state->allEffects->effectArr[3]->start = state->time;
		}
	}
	else if (inputChar == KEY_LEFT) {
		//allSprites.spriteArr[0]->xLoc += -10;
		pShip->xAcc += -baseThrust*(1e6)/REFRESH_RATE;
		if (state->score < LEVEL_THREE_SCORE) {
			state->allEffects->effectArr[1]->start = state->time;
		}
	}
	else if (inputChar == KEY_DOWN) {
		//allSprites.spriteArr[0]->yLoc += 1;
		pShip->yAcc += baseThrust*(1e6)/REFRESH_RATE;
		if (state->score < LEVEL_THREE_SCORE) {
			state->allEffects->effectArr[4]->start = state->time;
			state->allEffects->effectArr[5]->start = state->time;
		}
	}
	else if (inputChar == KEY_RIGHT) {
		//allSprites.spriteArr[0]->xLoc += 1;
		pShip->xAcc += baseThrust*(1e6)/REFRESH_RATE;
		if (state->score < LEVEL_THREE_SCORE) {
			state->allEffects->effectArr[0]->start = state->time;
		}
	}
	else if (inputChar == ' ' && pShip->isShooter > 0) {
		// player missile (only one at a time here for testing)
		if (pShip->wpnSelect==-1) {
			pShip->isShooter -= 1;
			addSprite(missileRt, state, lib);
			// if you want to be real physicsy then you'd actually copy the pShip
			// velocities to the new sprite as well
			modSprite(-1, pShip->xLoc+ pShip->xCoM+5, pShip->yLoc+pShip->yCoM, 45*(1e6)/REFRESH_RATE, 0, 0, state);
		}
		// player laser
		else if (pShip->wpnSelect==1) {
			pShip->isShooter -= 1;
			addSprite(laser, state, lib);
			modSprite(-1, pShip->xLoc+ pShip->xCoM, pShip->yLoc+pShip->yCoM, 150*(1e6)/REFRESH_RATE, 0, 0, state);
			//struct sprite * laserPtr = state->allSprites->spriteArr[state->allSprites->numSprites-1];
			addEffect(laserEffect,state->allSprites->numSprites-1,state, lib);
			modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y

			}
	}
	flushinp();
}

void restrictPlaySpace(struct gameState *state) {
	struct sprite *player = state->allSprites->spriteArr[0];

	// TODO: Determine this based on CoM and radius?
	int playerHeight = 4;
	int playerWidth = 24;

	if (player->xLoc <= 0) {
		// Left border.

		// Ensure the player didn't slip past the boundary.
		player->xLoc = 0;

		// Only allow positive acceleration / velocity.
		if (player->xVel < 0) {
			player->xVel = 0;
		}
		if (player->xAcc < 0) {
			player->xAcc = 0;
		}
	} else if (player->xLoc + playerWidth >= state->maxX) {
		// Right border.

		// Ensure the player didn't slip past the boundary.
		player->xLoc = state->maxX - playerWidth;

		// Only allow negative acceleration / velocity.
		if (player->xVel > 0) {
			player->xVel = 0;
		}
		if (player->xAcc > 0) {
			player->xAcc = 0;
		}
	}

	if (player->yLoc <= 0) {
		// Top border.

		// Ensure the player didn't slip past the boundary.
		player->yLoc = 0;

		// Only allow positive acceleration / velocity.
		if (player->yVel < 0) {
			player->yVel = 0;
		}
		if (player->yAcc < 0) {
			player->yAcc = 0;
		}
	} else if (player->yLoc + playerHeight >= state->maxY) {
		// Bottom border.

		// Ensure the player didn't slip past the boundary.
		player->yLoc = state->maxY - playerHeight;

		// Only allow negative acceleration / velocity.
		if (player->yVel > 0) {
			player->yVel = 0;
		}
		if (player->yAcc > 0) {
			player->yAcc = 0;
		}
	}
}

void createDummyWindows(struct gameState * state, int maxX, int maxY, WINDOW * dum1, WINDOW * dum2) {
	if (state->maxX < maxX) {
//		WINDOW *dum1 = newwin(maxY, localMax, 0, maxX);
//		dum1 = newwin(maxY, localMax, 0, maxX);
		wresize(dum1, maxY-0, maxX-state->maxX);
		mvwin(dum1, 0, state->maxX);
		if (state->maxY < maxY) {		
//			dum2 = newwin(maxY, state->maxX,state->maxY, 0);
			wresize(dum2, maxY-state->maxY, state->maxX-0);
			mvwin(dum2, state->maxY, 0);
		}
		else {
			//dum2 = newwin(-2,-2,-1,-1);
		}
	}
	else {
		if (state->maxY < maxY) {		
//			dum1 = newwin(maxY, state->maxX,state->maxY, 0);
			wresize(dum1, maxY-state->maxY, state->maxX-0);
			mvwin(dum1, state->maxY, 0);
		}
		else {
			//dum1 = newwin(-2,-2,-1,-1);
		}
		//dum2 = newwin(-2,-2,-1,-1);
	}
	
}
	
void scrubInput(int vertCtrl, int * inputChar) {
	if (vertCtrl && (*inputChar == KEY_LEFT || *inputChar == KEY_RIGHT)) {
		*inputChar = -1;
	}
	else if (!vertCtrl && (*inputChar == KEY_UP || *inputChar == KEY_DOWN)) {
		*inputChar = -1;
	}
	
}