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
    // Open highscores file.
    FILE *highscoresFile = fopen(HIGHSCORES_FILENAME, "r");

    if (highscoresFile == NULL) {
        // First score, we can just create the highscores file and write directly to it.
        highscoresFile = fopen(HIGHSCORES_FILENAME, "w");
        if (highscoresFile == NULL) {
            fprintf(stderr, "Error creating file for highscores.\n");
            exit(1);
        }

        fprintf(highscoresFile, "%d %s\n", score.score, score.name);

        fclose(highscoresFile);
        return;
    }

    // Need to copy contents of highscores file to temporary file, along with the new score inserted.

    // Create temporary file.
    FILE *tmpFile = fopen(TEMP_HIGHSCORES_FILENAME, "w");
    if (tmpFile == NULL) {
        fprintf(stderr, "Error creating temporary file for highscores.\n");
        exit(1);
    }

    // Start by finding where in the file we need to insert the new score.
    long insertPos = 0;
    struct highscore s;
    while (fscanf(highscoresFile, "%d %s\n", &s.score, s.name) != EOF) {
        if (s.score <= score.score) {
            break;
        }
        insertPos = ftell(highscoresFile);
    }

    // Copy everything before that point to the temporary file.
    int bufSize = 1024;
    char buf[bufSize];
    long bytesCopied = 0;
    int bytesToCopy;
    int bytesRead;
    fseek(highscoresFile, 0L, SEEK_SET);
    while (bytesCopied < insertPos) {
        bytesToCopy = insertPos - bytesCopied >= bufSize ? bufSize : insertPos - bytesCopied;

        bytesRead = fread(buf, sizeof(char), bytesToCopy, highscoresFile);
        fwrite(buf, sizeof(char), bytesRead, tmpFile);

        bytesCopied += bytesRead;
    }

    // Write the new score to the temporary file.
    fprintf(tmpFile, "%d %s\n", score.score, score.name);
    fseek(tmpFile, 0L, SEEK_END);

    // Copy the remaining highscores.
    while (1) {
        bytesRead = fread(buf, sizeof(char), bufSize, highscoresFile);
        if (bytesRead == 0) {
            break;
        }
        fwrite(buf, sizeof(char), bytesRead, tmpFile);

        bytesCopied += bytesRead;
    }

    fclose(highscoresFile);
    fclose(tmpFile);

    remove(HIGHSCORES_FILENAME);
    rename(TEMP_HIGHSCORES_FILENAME, HIGHSCORES_FILENAME);
}
