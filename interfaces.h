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
//#define REFRESH_RATE 50000
//#define REFRESH_RATE 100000
#define REFRESH_RATE 75000

// this is only so high because you aren't cleaning them out as you go
#define MAX_EFFECTS 1024
#define MAX_EFFECT_DISPS 16

#define MAX_SPRITES 8192
#define MAX_SPRITE_DISPS 16
#define MAX_SPRITE_EFFECTS 16

#define MAX_LEVEL_DISPS 256

enum STYPE {
ship = 0, eny1, openSpace1, openSpace2, openSpace3, openSpace4, missleRt, missleLt, laser,
asteroid1, asteroid2, asteroid3, sky1, sky2, gnd1
};

enum ETYPE {
	rtThrust = 0, ltThrust, upThrust1, upThrust2, dwThrust1, dwThrust2, shipEx1, laserEffect
};


/** SPRITES **/
struct sprite {
	int type;		// 0 for player, 1 for enemy ship, 2 for projectile, 3 for non-interacting background, 4 for indestructible
	// x,y location will indicate the top left of the sprite by convention
	float xLoc;
	float yLoc;
	// physics here or it's own struct?
	float xVel;
	float yVel;
	float xAcc;
	float yAcc;
	// collision detection, center of mass & radius
	float xCoM;
	float yCoM;
	float radius;
	// color? color by character, probably?
	// overall size (for collision detection)?
	// thruster/effects locations?
	int numDisps;
	struct dispPair *dispArr[MAX_SPRITE_DISPS];
	
	int effectIDs[MAX_SPRITE_EFFECTS];
	int numEffects;
	
	int markedForDeath;
	int AI;
};

struct spriteList {
	int numSprites;
	struct sprite *spriteArr[MAX_SPRITES];
};



/** EFFECTS **/
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
	
	int xSize;
	int ySize;
};

struct effectList {
	int numEffects;
	struct effect *effectArr[MAX_EFFECTS];
};


/** GAME STRUCTURES **/
struct gameState {
	struct spriteList * allSprites;
	struct effectList * allEffects;
	float time;
	float timeLast;
	int score;
	int deltaKills;
	float scoreTimeLast;
	// are we going to need this to adjudicate between the minimum console
	// size of all participants?
	int maxX;
	int maxY;
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
	int skyRate;
	int skyLimit;
	int groundVel;
	
	int groundOK;
	float maxHeight;
	
	// maybe you want levels to have single char disps?
	// that might be easier to procedurally generate
	int numDisps;
	struct dispPair *dispArr[MAX_LEVEL_DISPS];	
	
};

#endif