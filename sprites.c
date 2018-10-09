/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: sprites implementation file (any on screen, physics based object)

NOTES
1.  i think i'm going to have to use an 'effects' like structure here too, if any i want
any of the sprites to have multiple colors
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
//#include "gamePlay.h"
//#include "menu.h"
#include "sprites.h"




void initSprite(struct sprite *inputSprite, int ID, float xLoc, float yLoc, const char * disp) {
	inputSprite->ID = ID;
	inputSprite->xLoc = xLoc;
	inputSprite->yLoc = yLoc;
	strcpy((inputSprite->disp), disp);
	// init physics to zero
	inputSprite->xVel = 0;
	inputSprite->yVel = 0;
	inputSprite->xAcc = 0;
	inputSprite->yAcc = 0;
	//inputSprite->mass = yLoc; // for debugging
	
}


// instead of this, i think i would like to read these from a file initially or something
void initSpriteList(struct spriteList *localList) {
	localList->numSprites =0;
	
	// generate 1 sprite to test with
	struct sprite * ship = malloc(sizeof(struct sprite));
	initSprite(ship, 0, 50,20,"-----      -------------\n    \\      /\n   /-------/\\\n   \\--------/");
//	initSprite(ship, 0, 50,20, "O");
	localList->spriteArr[0] = ship;
	localList->numSprites++;
	
	// generate more sprites!
	// .. //

}

void printSprite(WINDOW* window, struct sprite* spriteIn) {
	// decompose the display string to print through the vertical axis
	// you can't just you /n in curses because it will start all the back at the front of the line, not
	// where you started printing
	int i=0, j=0, n=0;
	//int dum=0;
	char temp[256];
	for (i=0; i<= strlen(spriteIn->disp); i++) {
		// render each line individually, but keep track of where you left off in the sprite->disp with 'n'
		if (spriteIn->disp[i] == '\n' || spriteIn->disp[i] == '\0') {
			memcpy(temp, &(spriteIn->disp[n]), (i-n)*sizeof(char));
			// manually terminate your new string
			temp[i-n] = '\0';
			// print to buffer but don't refresh here
			mvwprintw(window, spriteIn->yLoc+j, spriteIn->xLoc, temp);
			// this is i+1 to skip the newline character in the next substring
			n=i+1;
			j++;
		}
	}
	
}


void freeSpriteList(struct spriteList *localList) {
	int i=0;
	for (i=0; i< localList->numSprites; i++) {
		free(localList->spriteArr[i]);
		localList->spriteArr[i] = NULL;
	}
}
