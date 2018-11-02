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
	addEffect(6, 0, state, lib);

	// init the level
	initLevelData(level);

	// init state variables
	state->deltaKills = 0;
	state->timeLast=-REFRESH_RATE/1e6;
	state->time = 0;
	state->score = 0; //45;  // debugging!
	state->scoreTimeLast =0;
	state->maxX=1;
	state->maxY=1;



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
void playGame(int network_socket) {
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
	state->maxX = maxX;
	state->maxY = maxY-titleSize;
	state->gndHeight = maxY;

	// init background (only needed for level 1, will fly into other backgrounds)
	initOpenSpaceBG(state, lib);

	int inputChar;

	// TODO: Remove
	struct gameState *dummy_state = malloc(sizeof(struct gameState));
	struct library *dummy_lib = malloc(sizeof(struct library));
	struct levelData *dummy_level = malloc(sizeof(struct levelData));

	// sets the blocking timer for wgetch
	wtimeout(action, 1);

	// init timing function
	struct timespec timeHold;
	clock_gettime(CLOCK_MONOTONIC, &timeHold);
	float tstart = timeHold.tv_sec + timeHold.tv_nsec / 1e9;

	// main play loop, will modify flag on player death or quit
	 while (playFlag) {
		//tstart = clock();

		inputChar = wgetch(action);

		// The value of network_socket acts as a flag for multiplayer / single player.
		if (network_socket > 0) {
			// Multiplayer.

			// Receive updates from the server.
			receive_data(network_socket, dummy_state, dummy_lib, dummy_level);

			// Send our input to the server (if invalid the server should just reject it).
			send_data(network_socket, &inputChar, sizeof(int));
		}

		handleInput(inputChar, &playFlag, state, lib);

		restrictPlaySpace(state);

		// procGen()
		procGen(state, lib, level, action);

		// updatePlayer()

		// updateSprites()
		updateSpriteAI(state, lib);
		updatePhysics(state);

		// collisionDetection()
		detectCollision(state, lib, level);

		// calcScore()
		calcScore(state, level);

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
		mvwprintw(title, 2, 1, "xMax:%i",state->maxX);

		mvwprintw(title, 0, 20, "numEnemies:%i",level->numEnemies);
		mvwprintw(title, 1, 20, "numSprites:%i",state->allSprites->numSprites);
		//mvwprintw(title, 2, 20, "yVel:%f",state->allSprites->spriteArr[0]->xAcc);

		mvwprintw(title, 0, 40, "numEffects:%i",state->allEffects->numEffects);
		//mvwprintw(title, 1, 40, "numDisps eff6:%i",state->allEffects->effectArr[6]->numDisps);
		//mvwprintw(title, 2, 40, "numDisps eff6:%i",lib->allEffects->effectArr[6]->numDisps);


		mvwprintw(title, 0, maxX - 15, "time: %.1f", round(state->time*10)/10);
		mvwprintw(title, 1, maxX - 15, "SCORE: %i", state->score);
		mvwprintw(title, 2, maxX - 15, "LEVEL: %i", level->currLevel);
		wrefresh(title);
		wrefresh(action);


		usleep(REFRESH_RATE);
		// generate the output game time (and time used for physics)
		state->timeLast=state->time;
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->time = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;
	}



	// clean up - do a better job of this!
	freeGame(state, lib, level);

	delwin(title);
	delwin(action);
}

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

// definitely doing too much here
// will probably refactor - fix me still noodling
void detectCollision(struct gameState * state, struct library * lib, struct levelData * level) {
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
				s2 = state->allSprites->spriteArr[j];
				if (s2->type != 3 && i != j && s2->markedForDeath == 0 && s2->type != s1->type) {
					dist = calcDistance(s1,s2);
					// if the distance between the sprites is within one of their spheres of influence
					// only then do a closer check for collision (expensive) (really only need the smaller sphere of influence, but this is safer)
					if (dist < s1->radius ||
						dist < s2->radius) {
						if (checkOverlap(s1, s2)) {
							//s1->xAcc += -20*(1e6)/REFRESH_RATE;
							// oh so klugey
							if (s1->type != 4 ) {
								addEffect(shipEx1,i,state, lib);
								modEffect(-1, state->time, s1->xCoM, s1->yCoM, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
								s1->markedForDeath=1;
							}
							if (s2->type !=4) {
								addEffect(shipEx1,j,state, lib);
								modEffect(-1, state->time, s2->xCoM, s2->yCoM, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y
								s2->markedForDeath=1;
							}
							if (s1->type == 1 || s2->type ==1) {
								state->deltaKills++;
							}

						}

					}
				}
			}
		}
	}
}
/// ***********//
// should probably move physics to its own header and implementation file
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
				while (dx <= strlen(temp)) {
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
		if (level->currLevel == 1) {
			state->score += (ppL1Kill*state->deltaKills);
		}
		else if (level->currLevel == 2) {
			state->score += (ppL2Kill*state->deltaKills);
		}
		else if (level->currLevel == 3) {
			state->score += (ppL3Kill*state->deltaKills);
		}
		state->deltaKills = 0;
	}
}

void waitQueue() {
	messageScreen("Now connecting...");

	int network_socket = establish_connection("128.193.36.41", 2997);

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

	if (inputChar == 'q') {
		*playFlag = 0;
	}
	// FOR PLAYER CONTROLS - consider:
						// limiting total velocity
						// increasing deltaAcc for counter thrust
						// applying counter acc at window borders -- Done
						// limiting limits at window borders -- Done
	else if (inputChar == KEY_UP) {
		//allSprites.spriteArr[0]->yLoc += -1;
		pShip->yAcc += -4*(1e6)/REFRESH_RATE;
		state->allEffects->effectArr[2]->start = state->time;
		state->allEffects->effectArr[3]->start = state->time;
	}
	else if (inputChar == KEY_LEFT) {
		//allSprites.spriteArr[0]->xLoc += -10;
		pShip->xAcc += -4*(1e6)/REFRESH_RATE;
		state->allEffects->effectArr[1]->start = state->time;
		}
	else if (inputChar == KEY_DOWN) {
		//allSprites.spriteArr[0]->yLoc += 1;
		pShip->yAcc += 4*(1e6)/REFRESH_RATE;
		state->allEffects->effectArr[4]->start = state->time;
		state->allEffects->effectArr[5]->start = state->time;
	}
	else if (inputChar == KEY_RIGHT) {
		//allSprites.spriteArr[0]->xLoc += 1;
		pShip->xAcc += 4*(1e6)/REFRESH_RATE;
		state->allEffects->effectArr[0]->start = state->time;
	}
	else if (inputChar == ' ') {
		// player missle (only one at a time here for testing)
		if (0) {
			addSprite(missleRt, state, lib);
			// if you want to be real physicsy then you'd actually copy the pShip
			// velocities to the new sprite as well
			modSprite(-1, pShip->xLoc+ pShip->xCoM, pShip->yLoc+pShip->yCoM, 30*(1e6)/REFRESH_RATE, 0, 0, state);
		}
		// player laser
		else {
			addSprite(laser, state, lib);
			modSprite(-1, pShip->xLoc+ pShip->xCoM, pShip->yLoc+pShip->yCoM, 150*(1e6)/REFRESH_RATE, 0, 0, state);
			//struct sprite * laserPtr = state->allSprites->spriteArr[state->allSprites->numSprites-1];
			addEffect(laserEffect,state->allSprites->numSprites-1,state, lib);
			modEffect(-1, state->time, 0, 0, state);	// this is effectIndex, start, x, y, state. use -999 to keep current x/y

			}
	}

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
