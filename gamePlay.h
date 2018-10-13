/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: gameplay header file
*******************************************************************************************/
#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "sprites.h"

#define DISP_SIZE 256


void updatePhysics(struct spriteList *local, float dt);

void playGame();
void waitQueue();

#endif