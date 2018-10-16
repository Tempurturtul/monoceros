/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  13 OCT 2018
** Description: gamePlay implementation file

NOTES
1. currently just a rough working sketch, need to clean up and move things around
2. need to determine game state structure and what needs to be passed to clients
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include <time.h>
#include <stdio.h>
#include <math.h>

#include "gamePlay.h"
#include "menu.h"
#include "sprites.h"
#include "effects.h"
#include "levels.h"





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
	
	// init state effects
	state->allEffects = malloc(sizeof(struct effectList));
	state->allEffects->numEffects = 0;
	addEffect(0, 0, state, lib);
	addEffect(1, 0, state, lib);
	addEffect(2, 0, state, lib);
	addEffect(3, 0, state, lib);
	addEffect(4, 0, state, lib);
	addEffect(5, 0, state, lib);
	
	// init the level
	initLevelData(level);
	
	usleep(REFRESH_RATE);
}

void freeGame(struct gameState * state, struct library * lib, struct levelData * level) {
	freeSpriteList(state->allSprites);
	freeEffectList(state->allEffects);

	freeSpriteList(lib->allSprites);
	freeEffectList(lib->allEffects);
	
	freeLevelDisps(level);

	free(state);
	free(lib);
	free(level);
	
}

//void playGame(struct gameState * state, struct library * lib, struct levelData * level) {
void playGame() {
	// Create windows.
	int maxX, maxY;
	int titleSize = 3;
	getmaxyx(stdscr, maxY, maxX);
	WINDOW *title = newwin(titleSize, maxX, 0, 0);
	WINDOW *action = newwin(maxY-titleSize, maxX, titleSize, 0);

	// Capture arrow key input.
	keypad(action, TRUE);

	int playFlag = 1;
	struct library * lib = malloc(sizeof(struct library));
	struct gameState * state = malloc(sizeof(struct gameState));
	struct levelData * level = malloc(sizeof(struct levelData));

	initGame(state, lib, level);

	//	int i=0;
//	float time=0, timeLast=0; 	// ugh do you really want this?  it's good to decouple the game updatePlayer
								// rate from the output refresh rate, but this could get rough fast - but might
								// help in synchronizing a laggy network connection
					
	int inputChar;
	
	// sets the blocking timer for wgetch
	wtimeout(action, 1);

	// init timing function 
	struct timespec timeHold;
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	float tstart = timeHold.tv_sec + timeHold.tv_nsec / 1e9;
	
	// main play loop, will modify flag on player death or quit
	 while (playFlag) {
		//tstart = clock();
		
		// getNetUpdates()
		// need to clean this up and and a switch for single or multiplayer controls
		// get this into its own function eventually
		// coupling effects with events feels klugey this way
		inputChar = wgetch(action);
		if (inputChar == 'q') {
			playFlag =0;
		}
		else if (inputChar == KEY_UP) {
			//allSprites.spriteArr[0]->yLoc += -1;
			state->allSprites->spriteArr[0]->yAcc += -2*(1e6)/REFRESH_RATE;
			state->allEffects->effectArr[2]->start = state->time;
			state->allEffects->effectArr[3]->start = state->time;
		}
		else if (inputChar == KEY_LEFT) {
			//allSprites.spriteArr[0]->xLoc += -10;
			state->allSprites->spriteArr[0]->xAcc += -2*(1e6)/REFRESH_RATE;
			state->allEffects->effectArr[1]->start = state->time;
			}
		else if (inputChar == KEY_DOWN) {
			//allSprites.spriteArr[0]->yLoc += 1;
			state->allSprites->spriteArr[0]->yAcc += 2*(1e6)/REFRESH_RATE;
			state->allEffects->effectArr[4]->start = state->time;
			state->allEffects->effectArr[5]->start = state->time;
		}
		else if (inputChar == KEY_RIGHT) {
			//allSprites.spriteArr[0]->xLoc += 1;
			state->allSprites->spriteArr[0]->xAcc += 2*(1e6)/REFRESH_RATE;
			state->allEffects->effectArr[0]->start = state->time;
		}
		
		// procGen()
		procGen(state, lib, level);
		
		// updatePlayer()
		
		// updateSprites()
		updatePhysics(state);
		
		// collisionDetection()
		
		// calcScore()
		
		// if collision 
			// something somthing
			
		// else
			// inc difficulty?
		
		
		// draw screen!
		
		// this all could be done elsewhere i think, make a separate func
		wclear(title);
		wclear(action);
		wcolor_set(action, 1, NULL);		// change this by referencing the appropriate colors in the sprites and effects themselves

		printSprite(action, state);
		printEffect(action, state);
		// i needed this for debugging
		mvwprintw(title, 0, 1, "xLoc:%f",state->allSprites->spriteArr[0]->xLoc);
		mvwprintw(title, 1, 1, "xVel:%f",state->allSprites->spriteArr[0]->xVel);
		mvwprintw(title, 2, 1, "xAcc:%f",state->allSprites->spriteArr[0]->xAcc);
		mvwprintw(title, 0, maxX - 15, "time: %f", state->time);
		wrefresh(title);
		wrefresh(action);
		

		usleep(REFRESH_RATE);
		// generate the output game time (and time used for physics)
		state->timeLast=state->time;
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->time = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;
	}
	
	
	
	// clean up - do a better job of this!
	// you moved this to the game init, so you need to do it where you called gameInit
	// but I think you really want to do game init here, anyways
	//freeSpriteList(state->allSprites);
	//freeEffectList(state->allEffects);
	freeGame(state, lib, level);

	delwin(title);
	delwin(action);
}

void updatePhysics(struct gameState * state) {
	float dt = state->time - state->timeLast;
	struct spriteList *local = state->allSprites;
	int i=0;
	for (i=0; i< local->numSprites; i++) {
		//local->spriteArr[i]->yAcc += 10;		// 10 is my trial grav constant
		local->spriteArr[i]->yVel += (local->spriteArr[i]->yAcc)*dt;
		local->spriteArr[i]->yLoc += ((local->spriteArr[i]->yVel)*dt+0.5*(local->spriteArr[i]->yAcc)*(dt*dt));
		local->spriteArr[i]->yAcc = 0;

		//local->spriteArr[i]->xAcc += 10;		// 10 is my trial grav constant
		local->spriteArr[i]->xVel += (local->spriteArr[i]->xAcc)*dt;
		local->spriteArr[i]->xLoc += (((local->spriteArr[i]->xVel)*dt+0.5*(local->spriteArr[i]->xAcc)*(dt*dt)));
		local->spriteArr[i]->xAcc = 0;

		}
}

void waitQueue() {
	int start = (int)time(NULL);
	int now = start;

	// Timeout after 6 seconds.
	while (now - start < 6) {
		loadingScreen("waiting for players", now - start);

		sleep(0.5);
		now = (int)time(NULL);
	}
}
