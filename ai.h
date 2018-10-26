/*******************************************************************************************
** Authors: Chris Spravka
** Date:  18 OCT 2018
** Description: enemy AI header file
*******************************************************************************************/
#ifndef AI_H
#define AI_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <arpa/inet.h>

#include "interfaces.h"

void updateSpriteAI(struct gameState * state, struct library * lib);
void calcMovement(int sIndex, struct gameState * state);
void trackP(int sIndex, struct gameState * state);
void trackPI(int sIndex, struct gameState * state);
#endif
