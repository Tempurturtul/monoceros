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
	
	inputEffect->xSize=0;
	inputEffect->ySize=0;

}

void modEffect(int effectIndex, float start, float xLoc, float yLoc, struct gameState * state) {
	if (effectIndex == -1) {
		effectIndex = state->allEffects->numEffects -1;
	}
	struct effect *e1 = state->allEffects->effectArr[effectIndex];
	if (e1->xLoc > -1000) {
		e1->xLoc = xLoc - e1->xSize/2;
	}
	if (e1->yLoc > -1000) {
		e1->yLoc = yLoc - e1->ySize/2;
	}	
	
	e1->start = start;

}

void initDispPair(struct effect *inputEffect, int cpIn, int attrIn, const char * dispIn) {
	// error/length checking?
	inputEffect->dispArr[inputEffect->numDisps] = malloc(sizeof(struct dispPair));
	inputEffect->dispArr[inputEffect->numDisps]->colorPair = cpIn;
	inputEffect->dispArr[inputEffect->numDisps]->attr = attrIn;
	strcpy(inputEffect->dispArr[inputEffect->numDisps]->disp, dispIn);
	inputEffect->numDisps++;
}

void setEffectSize(struct effect * effectIn, int x, int y) {
	effectIn->xSize = x;
	effectIn->ySize = y;
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
	
	//ship explosion 1
	struct effect * shipEx1 = malloc(sizeof(struct effect));
	shipEx1->parentID = 0;
	initEffect(shipEx1, 0, 1, -1,-1);					// effect, ID (not used), ttl, x, y
	initDispPair(shipEx1, 2, 0, "\n\n\n   o   \n\n\n");		// effect, colorPair, attr, char *
	initDispPair(shipEx1, 3, 0, "\n\n \\*O*/ \n -O*O- \n /*O*\\ \n\n");
	initDispPair(shipEx1, 1, 0, "\no-+|.o-\n*      '\n.      o\n*^+-|\\o\n");
	initDispPair(shipEx1, 1, A_DIM, ".......\n.      .\n.      .\n.      .\n.......");
	setEffectSize(shipEx1, 7, 8);
	localList->effectArr[localList->numEffects] = shipEx1;
	localList->numEffects++;

	//ship explosion 2
	struct effect * shipEx2 = malloc(sizeof(struct effect));
	shipEx1->parentID = 0;
	initEffect(shipEx2, 0, 3, 0,0);					// effect, ID (not used), ttl, x, y
	initDispPair(shipEx2, 4, 0, "            --\n           /\n         --/\\\n        ---/");		// effect, colorPair, attr, char *
	initDispPair(shipEx2, 1, A_DIM, "            **\n           *\n         ****\n        ****");		// effect, colorPair, attr, char *
	initDispPair(shipEx2, 1, A_DIM, "            **\n           *\n         ****\n        ****");		// effect, colorPair, attr, char *
	initDispPair(shipEx2, 14, 0,  "\n\n        O");		// effect, colorPair, attr, char *
	initDispPair(shipEx2, 3, A_BOLD,          "\n       \\|/\n       = =\n       /|\\");		// effect, colorPair, attr, char *
	initDispPair(shipEx2, 3, A_BOLD,    "     \\ || /\n         \n    -- ** --\n         \n     / || \\");		// effect, colorPair, attr, char *

	//setEffectSize(shipEx2, 7, 8);
	localList->effectArr[localList->numEffects] = shipEx2;
	localList->numEffects++;

	//ship explosion 3
	struct effect * shipEx3 = malloc(sizeof(struct effect));
	shipEx1->parentID = 0;
	initEffect(shipEx3, 0, 1.5, 0,0);					// effect, ID (not used), ttl, x, y
	initDispPair(shipEx3, 2, 0, "\n\n        -");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        \\");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        |");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        /");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        -");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        \\");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        |");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        /");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        -");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        \\");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        |");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        /");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        -");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        \\");		// effect, colorPair, attr, char *
	initDispPair(shipEx3, 2, 0, "\n\n        |");		// effect, colorPair, attr, char *

	//setEffectSize(shipEx2, 7, 8);
	localList->effectArr[localList->numEffects] = shipEx3;
	localList->numEffects++;

	
	//laser effect
	struct effect * lasEff = malloc(sizeof(struct effect));
	lasEff->parentID = 0;
	initEffect(lasEff, 0, 1, 5,5);					// effect, ID (not used), ttl, x, y
	char temp[MAX_DISP_SUBSIZE];
	strcpy(temp, "-");
	int i;;
	for (i=0; i< MAX_DISP_SUBSIZE-2; i++) {
		strcat(temp, "-");
	}
	initDispPair(lasEff, 8, 0, temp);		// effect, colorPair, attr, char *
	initDispPair(lasEff, 8, A_DIM, temp);
	initDispPair(lasEff, 1, A_DIM, temp);
	//setEffectSize(lasEff, 7, 8);
	localList->effectArr[localList->numEffects] = lasEff;
	localList->numEffects++;
	
	// ammo effect
	struct effect * ammo1 = malloc(sizeof(struct effect));
	shipEx1->parentID = 0;
	initEffect(ammo1, 0, 0.1, 0,0);					// effect, ID (not used), ttl, x, y
	initDispPair(ammo1, 1, 0, " ");		// effect, colorPair, attr, char *
	localList->effectArr[localList->numEffects] = ammo1;
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
	temp->parentID = parentID; // this doesn't stay pertinent for long
	state->allEffects->effectArr[state->allEffects->numEffects] = temp;
	// goes both ways - > effect->sprite and sprite-> effect, this is needed because how how frequently sprites are 
	// created and destroyed and index values change
	state->allSprites->spriteArr[parentID]->effectIDs[state->allSprites->spriteArr[parentID]->numEffects] = state->allEffects->numEffects;
	state->allSprites->spriteArr[parentID]->numEffects++;
	state->allEffects->numEffects++;

}


// rewrote to cycle through sprites first (more robust for timing!)
void printEffect(WINDOW* window, struct gameState * state) {
	int effectCount = 0, spriteCount=0;
	struct spriteList * localSpriteList = state->allSprites;
	if (state->skyReady && state->skyReady <3) {
		wbkgd(window, COLOR_PAIR(15));
		state->skyReady ++;
	}
	
	for (spriteCount=0; spriteCount < localSpriteList->numSprites; spriteCount++) {
		for (effectCount=0; effectCount < localSpriteList->spriteArr[spriteCount]->numEffects; effectCount++) {
			struct effect * effectIn = state->allEffects->effectArr[localSpriteList->spriteArr[spriteCount]->effectIDs[effectCount]];
			char tdisp[MAX_DISP_SUBSIZE];
			// results based on time
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
					// gross
					if (localSpriteList->spriteArr[spriteCount]->markedForDeath > 0) {
						localSpriteList->spriteArr[spriteCount]->markedForDeath=-1;
					}
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
						int dx=0;
						// this little ditty will prevent it from overprinting the spaces, so you don't get 
						// a big dumb background block around your sprite
						while (dx < strlen(temp)) {
							if (temp[dx] != ' ') {
								mvwprintw(window, localSpriteList->spriteArr[spriteCount]->yLoc + effectIn->yLoc+j, localSpriteList->spriteArr[spriteCount]->xLoc + effectIn->xLoc+(float)dx, "%c", temp[dx]);
							}
							dx++;
						}
						// this is i+1 to skip the newline character in the next substring
						n=i+1;
						j++;
					}
				}
			wattroff(window, effectIn->dispArr[dispIndex]->attr);
			}
		}
	}
}


void printEffectServer(struct gameState * state) {
	int effectCount = 0, spriteCount=0;
	struct spriteList * localSpriteList = state->allSprites;
	if (state->skyReady && state->skyReady <3) {
		state->skyReady ++;
	}
	for (spriteCount=0; spriteCount < localSpriteList->numSprites; spriteCount++) {
		for (effectCount=0; effectCount < localSpriteList->spriteArr[spriteCount]->numEffects; effectCount++) {
			struct effect * effectIn = state->allEffects->effectArr[localSpriteList->spriteArr[spriteCount]->effectIDs[effectCount]];
			char tdisp[MAX_DISP_SUBSIZE];
			// results based on time
			int dispIndex = 0;
			if (effectIn->start > 0) { 
				if  ((state->time-effectIn->start) < effectIn->ttl) {
					dispIndex = (int)effectIn->numDisps*((state->time-effectIn->start)/effectIn->ttl);
					strcpy(tdisp, effectIn->dispArr[dispIndex]->disp);
				}
				// turn it off and reset it
				else {
					strcpy(tdisp, "");
					effectIn->start = -1;
					// gross
					if (localSpriteList->spriteArr[spriteCount]->markedForDeath > 0) {
						localSpriteList->spriteArr[spriteCount]->markedForDeath=-1;
					}
				}
			}
		}
	}
}


// fix me - you need to regularly call this function and look for defunct effects
// in order to stay under the limit (and manage your memory better)
// i think you really want a linked-list here, so you don't lose track of folks
void delEffect(struct gameState * state, int parentIndex) {
	// fix and clean this up
	// find the effect
	struct sprite * spriteIn = state->allSprites->spriteArr[parentIndex];
	int i,j;
	for (i=0; i<spriteIn->numEffects; i++) {
		for (j=0; j<state->allEffects->effectArr[spriteIn->effectIDs[i]]->numDisps; j++) {
			free(state->allEffects->effectArr[spriteIn->effectIDs[i]]->dispArr[j]);
		}
		free(state->allEffects->effectArr[spriteIn->effectIDs[i]]);
	}
	// this only works because you are currently killing the last thing you
	// gave an effect to
	state->allEffects->numEffects--;
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
