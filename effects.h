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

#include "interfaces.h"


void initEffect(struct effect *inputEffect, int ID, float ttl,  float xLoc, float yLoc);
void initDispPair(struct effect *inputEffect, int cpIn, int attrIn, const char * dispIn);
//void initEffectList(struct effectList *localList);
void initEffectLibrary(struct effectList *localList);
void addEffect(int ID, int parentID, struct gameState * state, struct library * lib);
void printEffect(WINDOW* window, struct gameState * state);
void freeEffectList(struct effectList *localList);


#endif