/*******************************************************************************************
** Authors: Chris Spravka, Matthew Feidt
** Date:  16 OCT 2018
** Description: interfaces header file
1. a collection of all the interfacing structures and definitions in a more reasonable place
2. this ended up a lot worse than i envisioned
*******************************************************************************************/
#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>


#define MAX_DISP_SUBSIZE 64
#define REFRESH_RATE 50000

#define MAX_EFFECTS 16
#define MAX_EFFECT_DISPS 16

#define MAX_SPRITES 32
#define MAX_SPRITE_DISPS 16

#define MAX_LEVEL_DISPS 256

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
	// color? color by character, probably?
	// overall size (for collision detection)?
	// thruster/effects locations?
	int numDisps;
	struct dispPair *dispArr[MAX_SPRITE_DISPS];
};

struct spriteList {
	int numSprites;
	struct sprite *spriteArr[MAX_SPRITES];
};

struct effect {
	int ID;
	float start;
	float ttl;
	// x,y location will indicate the top left of the effect by convention
	float xLoc;
	float yLoc;
	// think about how you really want to do this, once you get it working
	// corresponding arrays of arrays of colors and characters?
	
	int numDisps;
	struct dispPair *dispArr[MAX_EFFECT_DISPS];
	
	int parentID;
};

struct effectList {
	int numEffects;
	struct effect *effectArr[MAX_EFFECTS];
};


struct gameState {
	struct spriteList * allSprites;
	struct effectList * allEffects;
	float time;
	float timeLast;
	int score;
};

struct library {
	struct spriteList * allSprites;
	struct effectList * allEffects;	
};

struct dispPair {
	int colorPair;
	char disp[MAX_DISP_SUBSIZE];
	//char * disp;
	int attr;
};

struct levelData {
	int currLevel;
	int numEnemies;
	int maxNumEnemies;
	int spawnOK;
	int AIlevel;
	
	// maybe you want levels to have single char disps?
	// that might be easier to procedurally generate
	int numDisps;
	struct dispPair *dispArr[MAX_LEVEL_DISPS];	
	
};

#endif