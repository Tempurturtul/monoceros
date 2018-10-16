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

#include "interfaces.h"


void initSprite(struct sprite *inputSprite, int ID, float xLoc, float yLoc);
void initDispPairSprite(struct sprite *inputSprite, int cpIn, int attrIn, const char *dispIn);
void initSpriteLibrary(struct spriteList *localList);
void addSprite(int ID, struct gameState * state, struct library * lib);

void printSprite(WINDOW* window, struct gameState * state);
void freeSpriteList(struct spriteList *localList);

#endif