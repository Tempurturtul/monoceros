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




void initSprite(struct sprite *inputSprite, int type, float xLoc, float yLoc) {
	inputSprite->type = type;
	inputSprite->xLoc = xLoc;
	inputSprite->yLoc = yLoc;
	//strcpy((inputSprite->disp), disp);
	// init physics to zero
	inputSprite->xVel = 0;
	inputSprite->yVel = 0;
	inputSprite->xAcc = 0;
	inputSprite->yAcc = 0;
	//inputSprite->mass = yLoc; // for debugging
	inputSprite->xCoM=1;
	inputSprite->yCoM=1;
	inputSprite->radius=1;
	
	inputSprite->numDisps=0;
	
	inputSprite->markedForDeath=0;
	inputSprite->AI=0;
	inputSprite->numEffects = 0;
	
	
}

void modSprite(int spriteIndex, float xLoc, float yLoc, float dxAcc, float dyAcc, int AI, struct gameState * state) {
	if (spriteIndex < 0) {
		spriteIndex = state->allSprites->numSprites-1;
	}
	struct sprite * temp = state->allSprites->spriteArr[spriteIndex];
	if (xLoc > 0) {
		temp->xLoc = xLoc;
	}
	if (yLoc > 0) {
		temp->yLoc = yLoc;
	}
	temp->xAcc += dxAcc;
	temp->yAcc += dyAcc;
	temp->AI = AI;
}

void calcCoM(struct sprite *spriteIn) {
	int dispCount;
	float xMax=0, yMax=0;
	char tdisp[MAX_DISP_SUBSIZE];
	char temp[MAX_DISP_SUBSIZE];
	// this is really the same logic from printSprite, but without the printing
	// you really should be making a sep function for this (and printEffect)
	for (dispCount=0; dispCount< spriteIn->numDisps; dispCount++) {
		int i=0, j=0, n=0;
		strcpy(tdisp, spriteIn->dispArr[dispCount]->disp);
		for (i=0; i<= strlen(tdisp); i++) {
			if (tdisp[i] == '\n' || tdisp[i] == '\0') {
				memcpy(temp, &(tdisp[n]), (i-n)*sizeof(char));
				// manually terminate your new string
				temp[i-n] = '\0';
				if (strlen(temp) > 0) {
					yMax++;
				}
				if (strlen(temp) > xMax) {
					xMax = strlen(temp);
				}
				n=i+1;
				j++;
			}
		}
	}
	spriteIn->radius = sqrt(xMax*xMax+yMax*yMax)/2.;
	spriteIn->xCoM = xMax/2;
	spriteIn->yCoM = yMax/2;
}

void delSprite(struct gameState * state, int index) {

	int i,j;
	int lastSprite = state->allSprites->numSprites-1;
	struct spriteList * list = state->allSprites;
	// first free the dispArr, then free the structure itself
	for (j=0; j<list->spriteArr[index]->numDisps; j++) {
		free(list->spriteArr[index]->dispArr[j]);
	}
	free(list->spriteArr[index]);
	// now move everyone down one slot
	for (i=index; i<lastSprite; i++) {
		list->spriteArr[i] = list->spriteArr[i+1];
	}
	list->numSprites--;
}

void initDispPairSprite(struct sprite *inputSprite, int cpIn, int attrIn, const char* dispIn) {
	// error/length checking?
	inputSprite->dispArr[inputSprite->numDisps] = malloc(sizeof(struct dispPair));
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
	// initSprite(sprite, type, x, y)
	initSprite(ship, 0, 50,20);
	// by convention,  start a newline with \n, don't terminate the previous one
	initDispPairSprite(ship, 1, A_DIM, "-----      -------------");
	initDispPairSprite(ship, 1, 0, "\n    \\      /\n   /-------/\\\n   \\--------/");
	calcCoM(ship);
	localList->spriteArr[localList->numSprites] = ship;
	localList->numSprites++;
	// ID=1
	struct sprite * eny1 = malloc(sizeof(struct sprite));
	initSprite(eny1, 1,100,20);
	initDispPairSprite(eny1, 5, 0, "    -+");
	initDispPairSprite(eny1, 1, 0, "\n    /");
	initDispPairSprite(eny1, 6, 0, "\n\n ----");
	initDispPairSprite(eny1, 5, A_BOLD, "\n\nO");
	initDispPairSprite(eny1, 1, 0, "\n\n\n    \\");
	initDispPairSprite(eny1, 5, 0, "\n\n\n\n    -+");
	calcCoM(eny1);
	localList->spriteArr[localList->numSprites] = eny1;
	localList->numSprites++;

	//ID=2
	struct sprite * NIbackground1 = malloc(sizeof(struct sprite));
	initSprite(NIbackground1, 3,10,10);
	initDispPairSprite(NIbackground1, 1, A_DIM, "+");
	localList->spriteArr[localList->numSprites] = NIbackground1;
	localList->numSprites++;
	//ID=3	
	struct sprite * NIbackground2 = malloc(sizeof(struct sprite));
	initSprite(NIbackground2, 3,10,10);
	initDispPairSprite(NIbackground2, 1, 0, "+");
	localList->spriteArr[localList->numSprites] = NIbackground2;
	localList->numSprites++;
	//ID=4
	struct sprite * NIbackground3 = malloc(sizeof(struct sprite));
	initSprite(NIbackground3, 3,10,10);
	initDispPairSprite(NIbackground3, 1, A_DIM, "*");
	localList->spriteArr[localList->numSprites] = NIbackground3;
	localList->numSprites++;
	//ID=5	
	struct sprite * NIbackground4 = malloc(sizeof(struct sprite));
	initSprite(NIbackground4, 3,10,10);
	initDispPairSprite(NIbackground4, 1, 0, "*");
	localList->spriteArr[localList->numSprites] = NIbackground4;
	localList->numSprites++;

	// generate more sprites!
	// .. //
}

void addSprite(int ID, struct gameState * state, struct library * lib) {
	// make a copy so different instantiations can behave differently
	struct sprite * temp = (struct sprite *)malloc(sizeof(struct sprite));
	memcpy(temp,lib->allSprites->spriteArr[ID],sizeof(struct sprite));
	// also make sure you are copying the disps, even though you have no intention of making them behave differently
	int i;
	for (i=0; i < temp->numDisps; i++) {
		struct dispPair * dpTemp = (struct dispPair *)malloc(sizeof(struct dispPair));
		memcpy(dpTemp, lib->allSprites->spriteArr[ID]->dispArr[i], sizeof(struct dispPair));
		temp->dispArr[i] = dpTemp;
	}
	state->allSprites->spriteArr[state->allSprites->numSprites] = temp;
	state->allSprites->numSprites++;
}

void printSprite(WINDOW* window, struct gameState * state) {
	int spriteCount = 0;
	for (spriteCount = state->allSprites->numSprites-1; spriteCount >=0 ; spriteCount--) {
		struct sprite * spriteIn = state->allSprites->spriteArr[spriteCount];
		char tdisp[MAX_DISP_SUBSIZE];
		// oh boy, essentially effects can only vary in color in time, and sprites can only vary in color in space
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
					int dx=0;
					// this little ditty will prevent it from overprinting the spaces, so you don't get 
					// a big dumb background block around your sprite
					while (dx < strlen(temp)) {
						if (temp[dx] != ' ') {
							mvwprintw(window, spriteIn->yLoc+j, spriteIn->xLoc+(float)dx, "%c", temp[dx]);
						}
						dx++;
					}
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
