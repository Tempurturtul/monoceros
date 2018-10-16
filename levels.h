/*******************************************************************************************
** Author: Chris Spravka
** Date:  15 OCT 2018
** Description: levels header file
*******************************************************************************************/
#ifndef LEVELS_H
#define LEVELS_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "interfaces.h"


void initLevelData(struct levelData * level);
void procGen(struct gameState * state, struct library * lib, struct levelData * level);
void freeLevelDisps(struct levelData * level);

#endif