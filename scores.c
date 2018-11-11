/*******************************************************************************************
** Authors: Matthew Feidt
** Date:  10 NOV 2018
** Description: scores header file
**
** Functions to save and retrieve high scores.
*******************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "scores.h"

void getScores(struct highscore *scores, int n, int offset) {
    int i;

    // Open file for reading.
    FILE *highscoresFile = fopen(HIGHSCORES_FILENAME, "r");
    if (highscoresFile == NULL) {
        // No scores.
        for (i = 0; i < n; i++) {
            scores[i].score = 0;
            strcpy(scores[i].name, "");
        }
        return;
    }

    // Read file and collect desired entries.
    struct highscore s;
    int realizedOffset = 0;
    int numStored = 0;

    // For every score in the file...
    while (fscanf(highscoresFile, "%d %s\n", &s.score, s.name) != EOF) {
        // Skip scores until we've reached the offset.
        if (realizedOffset < offset) {
            realizedOffset++;
            continue;
        }

        // Store scores until we've reached the desired number.
        if (numStored < n) {
            scores[numStored].score = s.score;
            strcpy(scores[numStored].name, s.name);
            numStored++;
        } else {
            break;
        }
    }

    // Clear any remaining scores.
    for (i = numStored; i < n; i++) {
        scores[i].score = 0;
        strcpy(scores[i].name, "");
    }

    fclose(highscoresFile);
}

void putScore(struct highscore score) {
    // Create or open scores file for reading and writing.
    FILE *highscoresFile = fopen(HIGHSCORES_FILENAME, "w+");
    if (highscoresFile == NULL) {
        fprintf(stderr, "Error opening or creating high scores file \"%s\".\n", HIGHSCORES_FILENAME);
        exit(1);
    }

    struct highscore s;
    long writePos = 0;

    // Read through the file until we reach a score less than this one.
    while (fscanf(highscoresFile, "%d %s\n", &s.score, s.name) != EOF) {
        if (s.score <= score.score) {
            break;
        }
        writePos = ftell(highscoresFile);
    }

    // Write our new score.
    fseek(highscoresFile, writePos, SEEK_SET);
    fprintf(highscoresFile, "%d %s\n", score.score, score.name);

    // Now we need to bump everything after it down a line.
    while (1) {
        fprintf(highscoresFile, "%d %s\n", s.score, s.name);

        fseek(highscoresFile, 0L, SEEK_CUR); // Synchronize file position for reads and writes (see FOPEN(3) man page).

        if (fscanf(highscoresFile, "%d %s\n", &s.score, s.name) == EOF) {
            break;
        };

        fseek(highscoresFile, 0L, SEEK_CUR); // Synchronize file position for reads and writes (see FOPEN(3) man page).
    }

    fclose(highscoresFile);
}
