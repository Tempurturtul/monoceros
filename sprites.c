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
	inputSprite->errLast = 0;
	inputSprite->isShooter = -1;
	inputSprite->wpnSelect = 0;
	
	
}

void modSprite(int spriteIndex, float xLoc, float yLoc, float dxAcc, float dyAcc, int AI, struct gameState * state) {
	if (spriteIndex < 0) {
		spriteIndex = state->allSprites->numSprites-1;
	}
	struct sprite * temp = state->allSprites->spriteArr[spriteIndex];
	if (xLoc > -999) {
		temp->xLoc = xLoc;
	}
	if (yLoc > -999) {
		temp->yLoc = yLoc;
	}
	temp->xAcc += dxAcc;
	temp->yAcc += dyAcc;
	temp->AI = AI;
}

void gndSprite(int spriteIndex, float xLoc, float yLoc, float xVel, float yVel, struct gameState * state) {
	if (spriteIndex < 0) {
		spriteIndex = state->allSprites->numSprites-1;
	}
	struct sprite * temp = state->allSprites->spriteArr[spriteIndex];
	if (xLoc > -999) {
		temp->xLoc = xLoc;
	}
	if (yLoc > -999) {
		temp->yLoc = yLoc;
	}
	temp->xVel = xVel;
	temp->yVel = yVel;
	
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

	// Player ship.
	/*
		
		|[
		}=->
		|[

	*/
	struct sprite * ship = malloc(sizeof(struct sprite));
	// initSprite(sprite, type, x, y)
	initSprite(ship, 0, 50,20);
	// by convention,  start a newline with \n, don't terminate the previous one
	// initDispPairSprite(ship, 14, 0, "-----      -------------");
	// initDispPairSprite(ship, 1, 0, "\n    \\      /\n   /-------/\\\n   \\--------/");//\n      |\n      |");
	initDispPairSprite(ship, 1, 0, " |-[\n}===->\n |-[");
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

	struct sprite * eny1a = malloc(sizeof(struct sprite));
	initSprite(eny1a, 1,100,20);
	initDispPairSprite(eny1a, 5, 0, "    -+");
	initDispPairSprite(eny1a, 1, 0, "\n    /");
	initDispPairSprite(eny1a, 13, 0, "\n\n ----");
	initDispPairSprite(eny1a, 5, A_BOLD, "\n\nO");
	initDispPairSprite(eny1a, 1, 0, "\n\n\n    \\");
	initDispPairSprite(eny1a, 5, 0, "\n\n\n\n    -+");
	calcCoM(eny1a);
	localList->spriteArr[localList->numSprites] = eny1a;
	localList->numSprites++;

	
	// ID=1
	// NEED NEW ENEMIES AND COLOR!!
	struct sprite * eny2 = malloc(sizeof(struct sprite));
	initSprite(eny2, 1,100,20);
	initDispPairSprite(eny2, 15, A_BOLD,      " __________/|");
	initDispPairSprite(eny2, 15, A_BOLD,   "\n/___\\    |__|=");
	initDispPairSprite(eny2, 15, A_BOLD,"\n\n     _\\___|    ");
	initDispPairSprite(eny2, 11, A_UNDERLINE,   "\n        .\n        .");
	
	calcCoM(eny2);
	localList->spriteArr[localList->numSprites] = eny2;
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
	//ID=6
	struct sprite * missRt = malloc(sizeof(struct sprite));
	initSprite(missRt, 2,10,10);
	initDispPairSprite(missRt, 2, 0, "o)");
	initDispPairSprite(missRt, 4, 0, " >=>");
	calcCoM(missRt);
	localList->spriteArr[localList->numSprites] = missRt;
	localList->numSprites++;
	//ID=7
	struct sprite * missLt = malloc(sizeof(struct sprite));
	initSprite(missLt, 2,10,10);
	initDispPairSprite(missLt, 4, 0, "<=< ");
	initDispPairSprite(missLt, 2, 0, "    (o");
	localList->spriteArr[localList->numSprites] = missLt;
	localList->numSprites++;
	//ID=8
	struct sprite * lasRt = malloc(sizeof(struct sprite));
	initSprite(lasRt, 4,10,10);
 	char temp[MAX_DISP_SUBSIZE];
	strcpy(temp, "-");
	int i;
	for (i=0; i< MAX_DISP_SUBSIZE-2; i++) {
		strcat(temp, "-");
	} 
	initDispPairSprite(lasRt, 7, 0, temp);
	calcCoM(lasRt);
	localList->spriteArr[localList->numSprites] = lasRt;
	localList->numSprites++;

	// ID=9
	struct sprite * ast1 = malloc(sizeof(struct sprite));
	initSprite(ast1, 2,0,0);
	initDispPairSprite(ast1, 1, A_DIM, "/\\-|\\");
	initDispPairSprite(ast1, 1, A_DIM, "\n[ .-*\\");
	initDispPairSprite(ast1, 1, A_DIM, "\n\n<8 o |");
	initDispPairSprite(ast1, 1, A_DIM, "\n\n\n {^ ,/\n");
	initDispPairSprite(ast1, 1, A_DIM, "\n\n\n\n  -\\/\n");
	calcCoM(ast1);
	localList->spriteArr[localList->numSprites] = ast1;
	localList->numSprites++;

	// ID=10
	struct sprite * ast2 = malloc(sizeof(struct sprite));
	initSprite(ast2, 2,0,0);
	initDispPairSprite(ast2, 9, A_DIM, " ooo ");
	initDispPairSprite(ast2, 9, A_DIM, "\nooooo");
	initDispPairSprite(ast2, 9, A_DIM, "\n\n ooo \n");
	calcCoM(ast2);
	localList->spriteArr[localList->numSprites] = ast2;
	localList->numSprites++;

	// ID=11
	struct sprite * ast3 = malloc(sizeof(struct sprite));
	initSprite(ast3, 2,0,0);
	initDispPairSprite(ast3, 1, 0, " /\\ ");
	initDispPairSprite(ast3, 1, 0, "\n|p_-");
	initDispPairSprite(ast3, 1, 0, "\n\n\\// \n");
	calcCoM(ast3);
	localList->spriteArr[localList->numSprites] = ast3;
	localList->numSprites++;

	// ID=12
	struct sprite * sk1 = malloc(sizeof(struct sprite));
	initSprite(sk1, 3,0,0);
	initDispPairSprite(sk1, 10, 0, "o");
	calcCoM(sk1);
	localList->spriteArr[localList->numSprites] = sk1;
	localList->numSprites++;
	
	// ID=13
	struct sprite * sk2 = malloc(sizeof(struct sprite));
	initSprite(sk2, 3,0,0);
	initDispPairSprite(sk2, 11, 0, "o");
	calcCoM(sk2);
	localList->spriteArr[localList->numSprites] = sk2;
	localList->numSprites++;

	// ID=14
	struct sprite * gnd1 = malloc(sizeof(struct sprite));
	initSprite(gnd1, 5,0,0);
	initDispPairSprite(gnd1, 12, 0, "-\n");
	calcCoM(gnd1);
	localList->spriteArr[localList->numSprites] = gnd1;
	localList->numSprites++;
/*
//	these just did not look right at all - let's use crates instead!
	struct sprite * ammoM = malloc(sizeof(struct sprite));
	initSprite(ammoM, 6,0,0);
	initDispPairSprite(ammoM, 1, 0, " _ \n/ \\\n| |\n| |\n\\_/\n/-\\");
	initDispPairSprite(ammoM, 15, 0, "\n\n M\n -");
	calcCoM(ammoM);
	localList->spriteArr[localList->numSprites] = ammoM;
	localList->numSprites++;

	struct sprite * ammoPC = malloc(sizeof(struct sprite));
	initSprite(ammoPC, 7,0,0);
	initDispPairSprite(ammoPC, 1, 0, "  ||\n  ||\n  ||\n (88)");
	initDispPairSprite(ammoPC, 15, 0, "\n  PC");
	calcCoM(ammoPC);
	localList->spriteArr[localList->numSprites] = ammoPC;
	localList->numSprites++;
*/
	struct sprite * ammoM = malloc(sizeof(struct sprite));
	initSprite(ammoM, 6,0,0);
	initDispPairSprite(ammoM, 20, 0, ". . \n . .\n. . ");
	initDispPairSprite(ammoM, 21, 0, " . .\n. . \n . .");
	initDispPairSprite(ammoM, 1, 0, "\n MA");
	calcCoM(ammoM);
	localList->spriteArr[localList->numSprites] = ammoM;
	localList->numSprites++;

	struct sprite * ammoPC = malloc(sizeof(struct sprite));
	initSprite(ammoPC, 7,0,0);
	initDispPairSprite(ammoPC, 21, 0, ". . \n . .\n. . ");
	initDispPairSprite(ammoPC, 20, 0, " . .\n. . \n . .");
	initDispPairSprite(ammoPC, 1, 0, "\n PC");
	calcCoM(ammoPC);
	localList->spriteArr[localList->numSprites] = ammoPC;
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
