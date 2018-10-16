/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: sprites implementation file (any on screen, physics based object)

NOTES
1.  sprites are essentially a copy/paste of the effects - i'd use one structure, but i want to be specific about
what has physics and what does not
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h> 
#include <math.h>

#include "sprites.h"




void initSprite(struct sprite *inputSprite, int ID, float xLoc, float yLoc) {
	inputSprite->ID = ID;
	inputSprite->xLoc = xLoc;
	inputSprite->yLoc = yLoc;
	//strcpy((inputSprite->disp), disp);
	// init physics to zero
	inputSprite->xVel = 0;
	inputSprite->yVel = 0;
	inputSprite->xAcc = 0;
	inputSprite->yAcc = 0;
	//inputSprite->mass = yLoc; // for debugging
	
	inputSprite->numDisps=0;
	
}
void initDispPairSprite(struct sprite *inputSprite, int cpIn, int attrIn, const char* dispIn) {
	// error/length checking?
	inputSprite->dispArr[inputSprite->numDisps] = malloc(sizeof(struct dispPair));
	//inputSprite->dispArr[inputSprite->numDisps]->disp = malloc(MAX_DISP_SUBSIZE*sizeof(char));
	inputSprite->dispArr[inputSprite->numDisps]->colorPair = cpIn;
	inputSprite->dispArr[inputSprite->numDisps]->attr = attrIn;
	strcpy(inputSprite->dispArr[inputSprite->numDisps]->disp, dispIn);
	inputSprite->numDisps++;
}

// instead of this, i think i would like to read these from a file initially or something
// A_STANDOUT, A_UNDERLINE, A_REVERSE, A_BLINK, A_DIM, A_BOLD, A_ALTCHARSET, A_CHARTEXT, 0 for no effect
void initSpriteLibrary(struct spriteList *localList) {
	//localList = malloc(sizeof(struct spriteList));

	localList->numSprites =0;

	// generate 1 sprite to test with
	struct sprite * ship = malloc(sizeof(struct sprite));
//	initSprite(ship, 0, 50,20,"-----      -------------\n    \\      /\n   /-------/\\\n   \\--------/");
	initSprite(ship, 0, 50,20);
	initDispPairSprite(ship, 1, A_DIM, "-----      -------------");
	initDispPairSprite(ship, 1, 0, "\n    \\      /\n   /-------/\\\n   \\--------/");
	localList->spriteArr[localList->numSprites] = ship;
	localList->numSprites++;
	
	struct sprite * eny1 = malloc(sizeof(struct sprite));
//	initSprite(ship, 0, 50,20,"-----      -------------\n    \\      /\n   /-------/\\\n   \\--------/");
	initSprite(eny1, 0,100,20);
	initDispPairSprite(eny1, 5, 0, "    -+");
	initDispPairSprite(eny1, 1, 0, "\n    /");
	initDispPairSprite(eny1, 6, 0, "\n\n ----");
	initDispPairSprite(eny1, 5, A_BOLD, "\n\nO");
	initDispPairSprite(eny1, 1, 0, "\n\n\n    \\");
	initDispPairSprite(eny1, 5, 0, "\n\n\n\n    -+");
	localList->spriteArr[localList->numSprites] = eny1;
	localList->numSprites++;



	// generate more sprites!
	// .. //
}

void addSprite(int ID, struct gameState * state, struct library * lib) {
	// make a copy so different instantiations can behave differently
	struct sprite * temp = malloc(sizeof(struct sprite));
	memcpy(temp,lib->allSprites->spriteArr[ID],sizeof(struct sprite));
	// also make sure you are copying the disps, even though you have no intention of making them behave differently
	int i;
	for (i=0; i < temp->numDisps; i++) {
		struct dispPair * dpTemp = malloc(sizeof(struct dispPair));
		memcpy(dpTemp, lib->allSprites->spriteArr[ID]->dispArr[i], sizeof(struct dispPair));
		temp->dispArr[i] = dpTemp;
	}
	state->allSprites->spriteArr[state->allSprites->numSprites] = temp;
	state->allSprites->numSprites++;
}

void printSprite(WINDOW* window, struct gameState * state) {
	int spriteCount = 0;
	for (spriteCount = 0; spriteCount < state->allSprites->numSprites; spriteCount++) {
		struct sprite * spriteIn = state->allSprites->spriteArr[spriteCount];
		char tdisp[MAX_DISP_SUBSIZE];
		// oh boy, essentially effects can only vary in color in time, and sprites can only vary in color in space
	//mvwprintw(window,20,20,"disp:%i, dt:%f, ttl:%f", dispIndex, time-effectIn->start, effectIn->ttl);
	//mvwprintw(window,10,20,"%s",spriteIn->dispArr[0]->disp);
		int dispCount = 0;
		for (dispCount=0; dispCount < spriteIn->numDisps; dispCount++) {
			strcpy(tdisp, spriteIn->dispArr[dispCount]->disp);
			wcolor_set(window, spriteIn->dispArr[dispCount]->colorPair, NULL);
			wattron(window, spriteIn->dispArr[dispCount]->attr);
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
					mvwprintw(window, spriteIn->yLoc+j, spriteIn->xLoc, temp);
					// this is i+1 to skip the newline character in the next substring
					n=i+1;
					j++;
				}
			}
			wattroff(window, spriteIn->dispArr[dispCount]->attr);
		}
	}
}


void freeSpriteList(struct spriteList *localList) {
	int i=0, j=0;
	//printf("numSprites:%i", localList->numSprites);
	for (i=0; i< localList->numSprites; i++) {
		//printf("\n%i,%i\n", i, j);
		for (j=0; j<localList->spriteArr[i]->numDisps; j++) {
			//printf("\n%i,%i\n", i, j);
			free(localList->spriteArr[i]->dispArr[j]);
			localList->spriteArr[i]->dispArr[j] = NULL;
		}
		free(localList->spriteArr[i]);
		localList->spriteArr[i] = NULL;
	};
}
