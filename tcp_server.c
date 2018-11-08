#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "interfaces.h"
#include "gamePlay.h"
#include "tcp_client.h"
#include "levels.h"
#include "ai.h"



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
  printf("listening on port %d\n", port);
  listen(server_socket, 2);

  /*holds client socket*/
  int client_socket;
  int client_socket2;
  
  // END NET INIT
  
	// BEGIN GAME INIT
	int maxX, maxY, maxX2, maxY2;
	int playFlag =1; // FIXME i think you probably want this to live in state
	
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
			printf("x1,y1:%i,%i\n", maxX, maxY);
			printf("x2,y2:%i,%i\n", maxX2, maxY2);
			printf("x,y:%i,%i\n", state->maxX, state->maxY);
			
			state->maxY -= state->titleSize;
			state->gndHeight = maxY;
			// init background (only needed for level 1, will fly into other backgrounds)
			initOpenSpaceBG(state, lib);
			
			// state initialzied, now send that state back to the clients
			printf("size of state struct: %lu\n", sizeof(struct gameState));
			printf("size of state: %lu\n", sizeof(*state));
			long unsigned int sizer;
			sizer = getSizeOfState(state);
			printf("size of state (calc): %lu\n", sizer);
			printf("num sprites: %i\n", state->allSprites->numSprites);
			send_all(client_socket, state, lib, level);
			send_all(client_socket2, state, lib, level);

		}
	

      int input1 = ' ';
	  int input2;

      while (input1 != 'q') {
        /*1st parameter is passing the socket we send data on*/
        /*2nd parameter is passing the data we want to send*/
        /*3rd parameter specifies the size of the message*/
		/*
        send(client_socket, state, sizeof(struct gameState), 0);
        send(client_socket2, state, sizeof(struct gameState), 0);
        send(client_socket, lib, sizeof(struct library), 0);
        send(client_socket2, lib, sizeof(struct library), 0);
        send(client_socket, level, sizeof(struct levelData), 0);
        send(client_socket2, level, sizeof(struct levelData), 0);
		*/

		// FIXME modify this so that both inputs can be valid
        recv(client_socket, &input1, sizeof(int), 0);
        printf("input1: %d\n", input1);
        recv(client_socket2, &input2, sizeof(int), 0);
        printf("input2: %d\n", input2);
		if (input2 == 'q') {
			input1= 'q';
		}
		
		handleInput(input1, &playFlag, state, lib);
		handleInput(input2, &playFlag, state, lib);

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
		
		// BLOCK HERE
		printf("block!\n");
		usleep(75000);
		send_all(client_socket, state, lib, level);
		send_all(client_socket2, state, lib, level);
		printf("sent!\n");

//		input = 'q';
      }
      if (input1 == 'q') {
        break;
      }
    }
  }
	
	// clean up
	freeGame(state, lib, level);

  /*finished basic server app, time to close the socket*/
  close(server_socket);

  return 0;
}


