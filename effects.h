/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: effects header file
*******************************************************************************************/
#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "sprites.h"

#define MAX_EFFECTS 256

struct effect {
	int ID;
	float start;
	float ttl;
	// x,y location will indicate the top left of the effect by convention
	float xLoc;
	float yLoc;
	// think about how you really want to do this, once you get it working
	// corresponding arrays of arrays of colors and characters?
	char disp1[16];
	char disp2[16];
	char disp3[16];
	
	int cp1;
	int cp2;
	int cp3;
	
	int parentID;
};

struct effectList {
	int numEffects;
	struct effect *effectArr[MAX_EFFECTS];
};


void initEffect(struct effect *inputEffect, int ID, float ttl,  float xLoc, float yLoc, const char * disp1, const char * disp2, const char * disp3, int cp1, int cp2, int cp3);
void initEffectList(struct effectList *localList);
void printEffect(WINDOW* window, struct effect* effectIn, struct spriteList * localSpriteList, float time);
void freeEffectList(struct effectList *localList);


#endif