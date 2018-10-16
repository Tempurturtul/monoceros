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


#include "effects.h"




void initEffect(struct effect *inputEffect, int ID, float ttl,  float xLoc, float yLoc) {
	inputEffect->ID = ID;
	inputEffect->ttl = ttl;
	inputEffect->xLoc = xLoc;
	inputEffect->yLoc = yLoc;
	
	inputEffect->start =-1;
	
	inputEffect->numDisps=0;

}
void initDispPair(struct effect *inputEffect, int cpIn, int attrIn, const char * dispIn) {
	// error/length checking?
	inputEffect->dispArr[inputEffect->numDisps] = malloc(sizeof(struct dispPair));
	inputEffect->dispArr[inputEffect->numDisps]->colorPair = cpIn;
	inputEffect->dispArr[inputEffect->numDisps]->attr = attrIn;
	strcpy(inputEffect->dispArr[inputEffect->numDisps]->disp, dispIn);
	inputEffect->numDisps++;
}

// instead of this, i think i would like to read these from a file initially or something
void initEffectLibrary(struct effectList *localList) {
	localList->numEffects =0;
	
	// generate right thrust effect to test with
	struct effect * rtThrust = malloc(sizeof(struct effect));
	//initEffect(rtThrust, 0, 0.5, 0,2,"  o"," O ",")  ",2, 3, 1);
	initEffect(rtThrust, 0, 0.5, 0, 2);
	rtThrust->parentID = 0;
	initDispPair(rtThrust, 2, 0, "  o");
	initDispPair(rtThrust, 3, 0, " O ");
	initDispPair(rtThrust, 1, 0, ")  ");
	initDispPair(rtThrust, 1, A_DIM, "|  ");
	localList->effectArr[localList->numEffects] = rtThrust;
	localList->numEffects++;
	

	// left thrust
	struct effect * ltThrust = malloc(sizeof(struct effect));
	//initEffect(ltThrust, 0, 0.5, 13,2,"o  "," O ","  (",2, 3, 1);
	initEffect(ltThrust, 0, 0.5, 13,2);
	ltThrust->parentID = 0;
	initDispPair(ltThrust, 2, 0, "o  ");
	initDispPair(ltThrust, 3, 0, " O ");
	initDispPair(ltThrust, 1, 0, "  (");
	localList->effectArr[localList->numEffects] = ltThrust;
	localList->numEffects++;

	// up thrust 1
	struct effect * upThrust1 = malloc(sizeof(struct effect));
	//initEffect(upThrust1, 0, 0.5, 0,1,"o","\nO","\n\n_",2, 3, 1);
	initEffect(upThrust1, 0, 0.5, 0,1);
	upThrust1->parentID = 0;
	initDispPair(upThrust1, 2, 0, "o");
	initDispPair(upThrust1, 3, 0, "\nO ");
	initDispPair(upThrust1, 1, 0, "\n\n_");
	localList->effectArr[localList->numEffects] = upThrust1;
	localList->numEffects++;
	// up thrust 2
	struct effect * upThrust2 = malloc(sizeof(struct effect));
	//initEffect(upThrust2, 0, 0.5, 23,1,"o","\nO","\n\n_",2, 3, 1);
	initEffect(upThrust2, 0, 0.5, 23,1);
	upThrust2->parentID = 0;
	initDispPair(upThrust2, 2, 0, "o");
	initDispPair(upThrust2, 3, 0, "\nO ");
	initDispPair(upThrust2, 1, 0, "\n\n_");
	localList->effectArr[localList->numEffects] = upThrust2;
	localList->numEffects++;

	//down thrust 1
	struct effect * dwThrust1 = malloc(sizeof(struct effect));
	//initEffect(dwThrust1, 0, 0.5, 0,-3,"\n\no","\nO","-\n\n",2, 3, 1);
	initEffect(dwThrust1, 0, 0.5, 0,-3);
	dwThrust1->parentID = 0;
	initDispPair(dwThrust1, 2, 0, "\n\no");
	initDispPair(dwThrust1, 3, 0, "\nO ");
	initDispPair(dwThrust1, 1, 0, "-\n\n");
	localList->effectArr[localList->numEffects] = dwThrust1;
	localList->numEffects++;
	// down thrust 2
	struct effect * dwThrust2 = malloc(sizeof(struct effect));
	//initEffect(dwThrust2, 0, 0.5, 23,-3,"\n\no","\nO","-\n\n",2, 3, 1);
	initEffect(dwThrust2, 0, 0.5, 23,-3);
	dwThrust2->parentID = 0;
	initDispPair(dwThrust2, 2, 0, "\n\no");
	initDispPair(dwThrust2, 3, 0, "\nO ");
	initDispPair(dwThrust2, 1, 0, "-\n\n");
	localList->effectArr[localList->numEffects] = dwThrust2;
	localList->numEffects++;


	// generate more effects!
	// .. //

}

void addEffect(int ID, int parentID, struct gameState * state, struct library * lib) {
	// make a copy so different instantiations can behave differently
	struct effect * temp = malloc(sizeof(struct effect));
	memcpy(temp,lib->allEffects->effectArr[ID],sizeof(struct effect));
	// also make sure you are copying the disps, even though you have no intention of making them behave differently
	int i;
	for (i=0; i < temp->numDisps; i++) {
		struct dispPair * dpTemp = malloc(sizeof(struct dispPair));
		memcpy(dpTemp, lib->allEffects->effectArr[ID]->dispArr[i], sizeof(struct dispPair));
		temp->dispArr[i] = dpTemp;
	}
	temp->parentID = parentID;
	state->allEffects->effectArr[state->allEffects->numEffects] = temp;
	state->allEffects->numEffects++;
}



void printEffect(WINDOW* window, struct gameState * state) {
	int effectCount = 0;
	for (effectCount=0; effectCount < state->allEffects->numEffects; effectCount++) {
		struct effect * effectIn = state->allEffects->effectArr[effectCount];
		struct spriteList * localSpriteList = state->allSprites;
		char tdisp[MAX_DISP_SUBSIZE];
		// this is just a place holder until you figure out how you want to organize this
		// you'll probably just loop through based on 'numDisps' or something
		int dispIndex = 0;
		if (effectIn->start > 0) { 
			if  ((state->time-effectIn->start) < effectIn->ttl) {
				dispIndex = (int)effectIn->numDisps*((state->time-effectIn->start)/effectIn->ttl);
				//mvwprintw(window,20,20,"disp:%i, dt:%f, ttl:%f", dispIndex, time-effectIn->start, effectIn->ttl);			
				strcpy(tdisp, effectIn->dispArr[dispIndex]->disp);
				wcolor_set(window, effectIn->dispArr[dispIndex]->colorPair, NULL);
				wattron(window, effectIn->dispArr[dispIndex]->attr);
			}
			// turn it off and reset it
			else {
				strcpy(tdisp, "");
				effectIn->start = -1;
				wcolor_set(window, 1, NULL);
				wattroff(window, effectIn->dispArr[dispIndex]->attr);
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
}


void freeEffectList(struct effectList *localList) {
	int i=0, j=0;
	for (i=0; i< localList->numEffects; i++) {
		for (j=0; j<localList->effectArr[i]->numDisps; j++) {
			free(localList->effectArr[i]->dispArr[j]);
		}
		free(localList->effectArr[i]);
		localList->effectArr[i] = NULL;
	}
}
