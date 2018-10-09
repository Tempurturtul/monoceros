/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: sprites header file
*******************************************************************************************/
#ifndef SPRITES_H
#define SPRITES_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

//#include "gamePlay.h"

#define MAX_SPRITES 256

struct sprite {
	int ID;
	// x,y location will indicate the top left of the sprite by convention
	float xLoc;
	float yLoc;
	// physics here or it's own struct?
	float xVel;
	float yVel;
	float xAcc;
	float yAcc;
	float mass;
	char disp[256];
	// color? color by character, probably?
	// overall size (for collision detection)?
	// thruster/effects locations?
};

struct spriteList {
	int numSprites;
	struct sprite *spriteArr[MAX_SPRITES];
};

void initSprite(struct sprite *inputSprite, int ID, float xLoc, float yLoc, const char * disp);
void initSpriteList(struct spriteList *localList);
void printSprite(WINDOW* window, struct sprite* spriteIn);
void freeSpriteList(struct spriteList *localList);

#endif