/*******************************************************************************************
** Author: Chris Spravka
** Date:  09 OCT 2018
** Description: menu implementation file
*******************************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include "menu.h"


int getMenu(WINDOW *title, WINDOW *action) {
	char chInput=' ';
	const char * gameTitle = "<insert>AWESOME GAME TITLE";
	const char * selectionStr = "Press number for selection:\n(1) single player\n(2) dual player\n(3) view high score\n(4) exit";
	int titleX = centerText(title, gameTitle);
	keypad(action, TRUE);
//	keypad(title, TRUE);


	//	wclear(stdscr);
	wclear(title);
	wclear(action);

	mvwprintw(title, 0, titleX, gameTitle);
	mvwprintw(action, 0,0, selectionStr);
	

//	mvwaddstr(title, 0, centerText(gameTitle), gameTitle);
//	mvwaddstr(action, 0,0, selections);

	wrefresh(title);
	wrefresh(action);
	
	while (needInput(action, &chInput)) {
		
		//	wclear(stdscr);
		//	wclear(title);
		//	wclear(action);
		mvwprintw(title, 0, titleX, gameTitle);
		mvwprintw(action, 0,0, selectionStr);
		wrefresh(title);
		wrefresh(action);

	}
	
	// 48 is the ascii value for 0
	return (chInput-48);
	
}

int centerText(WINDOW* window, const char * text) {
	int maxX, maxY;
	getmaxyx(window, maxY, maxX);
	
	return (maxX/2 - strlen(text));
}


int needInput(WINDOW * window, char * chInput_in) {
	*chInput_in = wgetch(window);
	// logic for testing a valid selection
	if (*chInput_in == '1' ||
		*chInput_in == '2' ||
		*chInput_in == '3' ||
		*chInput_in == '4') {
			return 0;
		}
	// otherwise return 1 (still need input)
	return 1;
}




void dispScores(WINDOW * title, WINDOW * action) {
	const char * gameStr = "showing those high scores!";
	int startX = centerText(action, gameStr);
	
	wclear(title);
	wclear(action);
	//mvwprintw(title, 0, titleX, gameTitle);
	mvwprintw(action, 5,startX, gameStr);
	wrefresh(title);
	wrefresh(action);	
	sleep(3);	
}