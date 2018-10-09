/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: effects implementation file (on screen, not-physics-based objects)

NOTES
1. figure out how you want to attach these to sprites (which seems logical)
2. you could instantiate new effect for every key punch/action/result, which would look cooler (appropriately persistent effects) but be 
an unholy mess of spawning/malloc'ing things and then killing them
3. do better with # of disps/cps and the timing between them

*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
//#include "gamePlay.h"
//#include "menu.h"
#include "effects.h"




void initEffect(struct effect *inputEffect, int ID, float ttl,  float xLoc, float yLoc, const char * disp1, const char * disp2, const char * disp3, int cp1, int cp2, int cp3) {
	inputEffect->ID = ID;
	inputEffect->ttl = ttl;
	inputEffect->xLoc = xLoc;
	inputEffect->yLoc = yLoc;
	strcpy((inputEffect->disp1), disp1);
	strcpy((inputEffect->disp2), disp2);
	strcpy((inputEffect->disp3), disp3);
	// init color pairs
	inputEffect->cp1 = cp1;
	inputEffect->cp2 = cp2;
	inputEffect->cp3 = cp3;
	
	inputEffect->start =-1;

	
}


// instead of this, i think i would like to read these from a file initially or something
void initEffectList(struct effectList *localList) {
	localList->numEffects =0;
	
	// generate right thrust effect to test with
	struct effect * rtThrust = malloc(sizeof(struct effect));
	initEffect(rtThrust, 0, 0.5, 0,2,"  o"," O ",")  ",2, 3, 1);
	rtThrust->parentID = 0;
	localList->effectArr[0] = rtThrust;
	localList->numEffects++;
	
	// left thrust
	struct effect * ltThrust = malloc(sizeof(struct effect));
	initEffect(ltThrust, 0, 0.5, 13,2,"o  "," O ","  (",2, 3, 1);
	ltThrust->parentID = 0;
	localList->effectArr[1] = ltThrust;
	localList->numEffects++;

	// up thrust 1
	struct effect * upThrust1 = malloc(sizeof(struct effect));
	initEffect(upThrust1, 0, 0.5, 0,1,"o","\nO","\n\n_",2, 3, 1);
	upThrust1->parentID = 0;
	localList->effectArr[2] = upThrust1;
	localList->numEffects++;
	// up thrust 2
	struct effect * upThrust2 = malloc(sizeof(struct effect));
	initEffect(upThrust2, 0, 0.5, 23,1,"o","\nO","\n\n_",2, 3, 1);
	upThrust2->parentID = 0;
	localList->effectArr[3] = upThrust2;
	localList->numEffects++;

	//down thrust 1
	struct effect * dwThrust1 = malloc(sizeof(struct effect));
	initEffect(dwThrust1, 0, 0.5, 0,-3,"\n\no","\nO","-\n\n",2, 3, 1);
	dwThrust1->parentID = 0;
	localList->effectArr[4] = dwThrust1;
	localList->numEffects++;
	// down thrust 2
	struct effect * dwThrust2 = malloc(sizeof(struct effect));
	initEffect(dwThrust2, 0, 0.5, 23,-3,"\n\no","\nO","-\n\n",2, 3, 1);
	dwThrust2->parentID = 0;
	localList->effectArr[5] = dwThrust2;
	localList->numEffects++;


	// generate more effects!
	// .. //

}

void printEffect(WINDOW* window, struct effect* effectIn, struct spriteList * localSpriteList, float time) {
	char tdisp[16];
	// this is just a place holder until you figure out how you want to organize this
	// you'll probably just loop through based on 'numDisps' or something
	if (effectIn->start > 0) { 
		if ((time-effectIn->start) < (effectIn->ttl/3)) {
			strcpy(tdisp, effectIn->disp1);
			wcolor_set(window, effectIn->cp1, NULL);
		}
		else if ((time-effectIn->start) < (effectIn->ttl*2/3)) {
			strcpy(tdisp, effectIn->disp2);		
			wcolor_set(window, effectIn->cp2, NULL);
		}
		else if ((time-effectIn->start) < (effectIn->ttl*3/3)) {
			strcpy(tdisp, effectIn->disp3);		
			wcolor_set(window, effectIn->cp3, NULL);
		}
		else {
			strcpy(tdisp, "");
			effectIn->start = -1;
			wcolor_set(window, 1, NULL);
		}
		// decompose the display string to print through the vertical axis
		// you can't just you /n in curses because it will start all the back at the front of the line, not
		// where you started printing
		int i=0, j=0, n=0;
		//int dum=0;
		char temp[256];
		for (i=0; i<= strlen(tdisp); i++) {
			// render each line individually, but keep track of where you left off in the sprite->disp with 'n'
			if (tdisp[i] == '\n' || tdisp[i] == '\0') {
				memcpy(temp, &(tdisp[n]), (i-n)*sizeof(char));
				// manually terminate your new string
				temp[i-n] = '\0';
				// print to buffer but don't refresh here
				mvwprintw(window, localSpriteList->spriteArr[effectIn->parentID]->yLoc + effectIn->yLoc+j, localSpriteList->spriteArr[effectIn->parentID]->xLoc + effectIn->xLoc, temp);
				// this is i+1 to skip the newline character in the next substring
				n=i+1;
				j++;
			}
		}
	}
}


void freeEffectList(struct effectList *localList) {
	int i=0;
	for (i=0; i< localList->numEffects; i++) {
		free(localList->effectArr[i]);
		localList->effectArr[i] = NULL;
	}
}
