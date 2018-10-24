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
#include <arpa/inet.h>

#include "interfaces.h"


void initLevelData(struct levelData * level);
void procGen(struct gameState * state, struct library * lib, struct levelData * level, WINDOW * window);
void addEnemy(struct gameState * state, struct library * lib, int ID, int AIlevel);
void freeLevelDisps(struct levelData * level);

void spawnEnemies(struct gameState * state, struct library * lib, struct levelData * level);

void manageSprites(struct gameState * state, struct library * lib, struct levelData * level);

void genOpenSpaceBG(struct gameState * state, struct library * lib);

void initOpenSpaceBG(struct gameState * state, struct library * lib);

void genAsteroidBG(struct gameState * state, struct library * lib, struct levelData * level);

int getRand(int, int);

#endif
