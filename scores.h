/*******************************************************************************************
** Authors: Matthew Feidt
** Date:  10 NOV 2018
** Description: scores header file
**
** Functions to save and retrieve high scores.
*******************************************************************************************/

#ifndef SCORES_H
#define SCORES_H

#define HIGHSCORES_FILENAME "highscores.txt"

struct highscore {
	int score;
	char name[11];
};

void getScores(struct highscore *scores, int n, int offset);
void putScore(struct highscore score);

#endif
