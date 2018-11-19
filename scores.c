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

    // Open files for reading.
    FILE *highscoresFile = fopen(HIGHSCORES_FILENAME, "r");
    FILE *mpHighscoresFile = fopen(MULTIPLAYER_HIGHSCORES_FILENAME, "r");

    // Read file and collect desired entries.
    struct highscore s;  // Next score to operate on.
    struct highscore sp; // Singleplayer score.
    struct highscore mp; // Multiplayer score.
    int realizedOffset = 0;
    int numStored = 0;
    // Flags indicating scores remain to be read.
    int outOfSPScores = highscoresFile == NULL ? 1 : 0;
    int outOfMPScores = mpHighscoresFile == NULL ? 1 : 0;
    // Flags indicating we should skip reading a score from a file.
    int skipSPScore = 0;
    int skipMPScore = 0;

    // For every score in the files...
    while (1) {
        // Read next score from files.
        if (!skipSPScore && !outOfSPScores) {
            if (fscanf(highscoresFile, "%d %s\n", &sp.score, sp.name) == EOF) {
                // No more singleplayer scores.
                outOfSPScores = 1;
            }
        }

        if (!skipMPScore && !outOfMPScores) {
            if (fscanf(mpHighscoresFile, "%d %s\n", &mp.score, mp.name) == EOF) {
                // No more multiplayer scores.
                outOfMPScores = 1;
            }
        }

        // Make sure a new score was read.
        if (outOfSPScores && outOfMPScores) {
            break;
        }

        // Reset skip flags.
        skipSPScore = 0;
        skipMPScore = 0;

        // Set the next score to operate on, and set a skip flag if necessary.
        if (!outOfSPScores && !outOfMPScores) {
            // Have to pick the higher of the two scores to use (we'll say sp score wins ties).
            s.score = sp.score >= mp.score ? sp.score : mp.score;
            strcpy(s.name, (sp.score >= mp.score ? sp.name : mp.name));
            // Now we have to skip lower score and reconsider it on the next loop.
            skipSPScore = sp.score >= mp.score ? 0 : 1;
            skipMPScore = !skipSPScore;
        } else if (outOfMPScores) {
            // Have to use sp score.
            s.score = sp.score;
            strcpy(s.name, sp.name);
        } else {
            // Have to use mp score.
            s.score = mp.score;
            strcpy(s.name, mp.name);
        }

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

    // Clean up.
    if (highscoresFile != NULL) {
        fclose(highscoresFile);
    }
    if (mpHighscoresFile != NULL) {
        fclose(mpHighscoresFile);
    }
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
