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
#include <arpa/inet.h>

#include "interfaces.h"


void initSprite(struct sprite *inputSprite, int ID, float xLoc, float yLoc);
void initDispPairSprite(struct sprite *inputSprite, int cpIn, int attrIn, const char *dispIn);
void initSpriteLibrary(struct spriteList *localList);
void addSprite(int ID, struct gameState * state, struct library * lib);
void modSprite(int spriteIndex, float xLoc, float yLoc, float dxAcc, float dyAcc, int AI, struct gameState * state);
void delSprite(struct gameState * state, int index);
void calcCoM(struct sprite *inputSprite);
void printSprite(WINDOW* window, struct gameState * state);
void freeSpriteList(struct spriteList *localList);

#endif
