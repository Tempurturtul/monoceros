/*******************************************************************************************
** Authors: Chad Erdwins
** Date:  12 OCT 2018
** Description: the game server
*******************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <time.h>

#include "interfaces.h"
#include "gamePlay.h"
#include "tcp_client.h"
#include "levels.h"
#include "effects.h"
#include "ai.h"
#include "menu.h"
#include "scores.h"


int main() {
  /*hold return value of our socket call*/
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    fprintf(stderr, "Error creating socket.\n");
    exit(1);
  }

  /*Define address structure*/
  struct sockaddr_in server_address;

  /*specifies the connection family*/
  server_address.sin_family = AF_INET;
  /*specifies the listening port*/
  int port = SERVER_IP_PORT;
  server_address.sin_port = htons(port);
  /*This is where you specify the IP Address. INADDR_ANY means any adress on the local machine*/
//  const char *ipaddr = "128.193.36.41";
//  const char *ipaddr = "192.168.0.18";
  const char *ipaddr = SERVER_IP_ADDR;

  server_address.sin_addr.s_addr = inet_addr(ipaddr);

  /*binding the socket to our specified IP and port*/
  if (bind(server_socket,(struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
    fprintf(stderr, "Error binding socket.\n");
    exit(1);
  }

  /*use the listen function to start listening to connections*/
  /*number corresponds to how many connections we want to have*/
  RENEW:printf("listening on port %d\n", port);
  listen(server_socket, 2);

  /*holds client socket*/
  int client_socket;
  int client_socket2;

  // END NET INIT

	// BEGIN GAME INIT
	int maxX, maxY, maxX2, maxY2;
	int vertCtrl = 1;
//	int count=0;

	struct gameState *state = malloc(sizeof(struct gameState));
	struct library *lib = malloc(sizeof(struct library));
	struct levelData *level = malloc(sizeof(struct levelData));

	initGame(state, lib, level);


  /*option to get the clients IP adress here, passed in NULL bc it isn't relevant in this case*/
  while ((client_socket = accept(server_socket, NULL, NULL))) {
    if ((client_socket2 = accept(server_socket, NULL, NULL))) {

		// only do this once, maxX inits to 1 (so will only occur on first run through)
		if (state->maxX ==1) {
			recv(client_socket, &maxX, sizeof(int), 0);
			recv(client_socket, &maxY, sizeof(int), 0);
			recv(client_socket2, &maxX2, sizeof(int), 0);
			recv(client_socket2, &maxY2, sizeof(int), 0);

			state->maxX = maxX;
			state->maxY = maxY;

			if (maxX2 < maxX) {
				state->maxX = maxX2;
			}
			if (maxY2 < maxY) {
				state->maxY = maxY2;
			}

			state->gndHeight = state->maxY;
			state->maxY = state->maxY - state->titleSize;

			// init background (only needed for level 1, will fly into other backgrounds)
			initOpenSpaceBG(state, lib);

			// state initialzied, now send that state back to the clients
//			printf("float(%lu), int(%lu), dispArrAddr(%lu), sprite(%lu), char(%lu), dispPair(%lu)\n", sizeof(float), sizeof(int), sizeof(struct dispPair *), sizeof(struct sprite), sizeof(char), sizeof(struct dispPair));
			send_all(client_socket, state, lib, level);
			send_all(client_socket2, state, lib, level);

			if (getRand(1,100)<50) {
				send_data(client_socket, &vertCtrl, sizeof(int));
				vertCtrl=0;
				send_data(client_socket2, &vertCtrl, sizeof(int));
			}
			else {
				send_data(client_socket2, &vertCtrl, sizeof(int));
				vertCtrl=0;
				send_data(client_socket, &vertCtrl, sizeof(int));
			}

		}


      int input1, input2;

		// init timing function
		struct timespec timeHold;
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		float tstart = timeHold.tv_sec + timeHold.tv_nsec / 1e9;


      while (state->playFlag) {


		// make sure both inputs are valid
        recv(client_socket, &input1, sizeof(int), 0);
        //printf("input1: %d\n", input1);
        recv(client_socket2, &input2, sizeof(int), 0);
        //printf("input2: %d\n", input2);
		
		handleInput(input1, state, lib);
		handleInput(input2, state, lib);

		restrictPlaySpace(state);

		// procedural level generation
		procGen(state, lib, level);

		// this updates the sprite actions (including firing)
		updateSpriteAI(state, lib);
		// update physics for all sprites
		updatePhysics(state);

		// handles collision detection and collision results
		detectCollision(state, lib);

		// calculates the current score, stores it in state
		calcScore(state, level);

		// this is bad and you should feel bad
		printEffectServer(state);

		// BLOCK HERE
//		usleep(75000);
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->timeWait = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;
		if (((1./12)-(state->timeWait-state->time))*1e6 > 0) {
			usleep(((1./12)-(state->timeWait-state->time))*1e6);
		}
		else {
			// do nothing! you're trying to catch up on frame rate!
			//printf("not keeping up %i\n", count);		/debugging!
			//count++;
		}
		// generate the output game time (and time used for physics)
		state->timeLast=state->time;
		clock_gettime(CLOCK_MONOTONIC, &timeHold);
		state->time = timeHold.tv_sec + timeHold.tv_nsec / 1e9 - tstart;

		// send it!
//		printf("sending!\n");
		send_all(client_socket, state, lib, level);
		send_all(client_socket2, state, lib, level);
//		printf("sent\n");
      }

		if (state->deathScreen) {
			while (state->deathScreen) {
				// Receive death screen inputs.
				recv(client_socket, &input1, sizeof(int), 0);
				recv(client_socket2, &input2, sizeof(int), 0);

				// Handle inputs.
				if (!handleDeathScreenInput(input1, state->playerName) || !handleDeathScreenInput(input2, state->playerName)) {
					// All done.
					state->deathScreen = false;
				}

				// Send updated player name and deathScreen flag.
				send_data(client_socket, state->playerName, sizeof(state->playerName));
				send_data(client_socket2, state->playerName, sizeof(state->playerName));
				send_data(client_socket, &state->deathScreen, sizeof(state->deathScreen));
				send_data(client_socket2, &state->deathScreen, sizeof(state->deathScreen));
			}

			// Record new highscore.
			struct highscore newHighscore;
			newHighscore.score = state->score;
			strcpy(newHighscore.name, state->playerName);
			putScore(newHighscore);

			// Send highscore file size.
			struct stat st;
			off_t highscoreFileSize = stat(HIGHSCORES_FILENAME, &st);
			send_data(client_socket, &highscoreFileSize, sizeof(off_t));
			send_data(client_socket2, &highscoreFileSize, sizeof(off_t));

			// Send highscore file.
			send_file(client_socket, HIGHSCORES_FILENAME);
			send_file(client_socket2, HIGHSCORES_FILENAME);
		}

    }
  }

	// clean up
	freeGame(state, lib, level);

  goto RENEW;

  /*finished basic server app, time to close the socket*/
  close(server_socket);

  return 0;
}
