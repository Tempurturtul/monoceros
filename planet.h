/*******************************************************************************************
** Author: Chris Spravka
** Date:  15 OCT 2018
** Description: planet level header file
*******************************************************************************************/
#ifndef PLANET_H
#define PLANET_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <arpa/inet.h>

#include "interfaces.h"



void initPlanetBG(struct gameState * state, struct library * lib, struct levelData * level);
void transitionPlanetBG(struct gameState * state, struct library * lib, struct levelData * level);
void swapSkyMotif(struct gameState * state, struct library * lib);
void genPlanetBG(struct gameState * state, struct library * lib, struct levelData * level);
void planetLevel(struct gameState * state, struct library * lib, struct levelData * level);
void setMaxHeight(struct levelData * level, struct gameState * state) ;

#endif
