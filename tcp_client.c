#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "interfaces.h"
#include "tcp_client.h"
#include "sprites.h"
#include "effects.h"

int establish_connection(const char *ip_address, int port) {
  int network_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (network_socket == -1) {
    // Error.
    return -1;
  }

  /* specify the address for the socket */
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  /* specify port */
  server_address.sin_port = htons(port);
  /* ip address connection (server_address) */
  server_address.sin_addr.s_addr = inet_addr(ip_address);

  /* calling the actual connect function here */
  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (connection_status == -1) {
    // Error.
    return -1;
  }

  return network_socket;
}

int receive_data(int network_socket, struct gameState *state, struct library *lib, struct levelData *level) {
//	printf("recv 1\n");
	int bytes_received = 0;
	//long unsigned int packageSize=0;	// debugging
	int i,j;

	// free what you had and start anew
	freeSpriteList(state->allSprites);
	freeEffectList(state->allEffects);

//printf("does this work?! %i\n", state->allSprites->numSprites);
	
	// first get some space for the sprite list
	if (state->allSprites == NULL) {
//		printf("oh you borked\n"); sleep(1);
	}
	else {
//		printf("oh hai\n");
	}
	//bytes_received += recv(network_socket, &packageSize, sizeof(long unsigned int), 0);	// debugging
	// receive the basic preallocated state
	bytes_received += recv(network_socket, state, sizeof(struct gameState), 0);
//printf("bytes: %i, ",bytes_received);
	state->allSprites = (struct spriteList *)malloc(sizeof(struct spriteList));
	// get num sprites so you can build spriteList
	bytes_received += recv(network_socket, &(state->allSprites->numSprites), sizeof(int), 0);
//int junk=0;
//bytes_received += recv(network_socket, &junk, sizeof(int), 0);
//printf("recv 2: bytes: %i\n", bytes_received);
//printf("numSprites: %i\n", junk);
//state->allSprites->numSprites = 1;
//printf("hate\n");
//printf("numSprites: %i\n", state->allSprites->numSprites);
	// loop through sprites, allocating memory as you go
	i=0;
	int junk = (int)state->allSprites->numSprites;
//printf("here again?!\n");
	for (i=0; i<state->allSprites->numSprites; i++) {
		state->allSprites->spriteArr[i] = malloc(sizeof(struct sprite));
//printf("sprite #:%i, ", i); fflush(stdout);
		// get that sprite!
		junk = recv(network_socket, state->allSprites->spriteArr[i], sizeof(struct sprite), 0);
//		printf("bytes for sprite: %i", junk);
		
		bytes_received += junk;
//printf("numDisps(%i), ", state->allSprites->spriteArr[i]->numDisps);fflush(stdout);
		// loop through disps in each sprite, allocating memory as you go
		for (j=0; j<state->allSprites->spriteArr[i]->numDisps; j++) {
			state->allSprites->spriteArr[i]->dispArr[j] = malloc(sizeof(struct dispPair));
//printf("disp #:%i, ", j);fflush(stdout);
			// get that disp!
			bytes_received += recv(network_socket, state->allSprites->spriteArr[i]->dispArr[j], sizeof(struct dispPair), 0);
		}
	}

//printf("done with sprites\n");sleep(5);
	// same deal for effects, start with the effectList
	state->allEffects = malloc(sizeof(struct effectList));
	bytes_received += recv(network_socket, &(state->allEffects->numEffects), sizeof(int), 0);
	// loop through effects allocating as you go
	for (i=0; i<state->allEffects->numEffects; i++) {
		state->allEffects->effectArr[i] = malloc(sizeof(struct effect));
		// get that effect!
		bytes_received += recv(network_socket, state->allEffects->effectArr[i], sizeof(struct effect), 0);
		// same as above, allocate for the disps, then populate
		for (j=0; j<state->allEffects->effectArr[i]->numDisps; j++) {
			state->allEffects->effectArr[i]->dispArr[j] = malloc(sizeof(struct dispPair));
			bytes_received += recv(network_socket, state->allEffects->effectArr[i]->dispArr[j], sizeof(struct dispPair), 0);
		}
	}
//printf("done with effects\n");	
//	don't have to deal with the library
	
	// level data is okay, no dynamic memory allocation, use as-is
	bytes_received += recv(network_socket, level, sizeof(struct levelData), 0);
//printf("done with levelData\n");	
	return bytes_received;
}

int send_data(int network_socket, void *payload, int payload_len) {
  int bytes_sent = send(network_socket, payload, payload_len, 0);

  return bytes_sent;
}

int send_all(int network_socket, struct gameState *state, struct library *lib, struct levelData *level) {
	int bytes_sent=0;
	//long unsigned int size = 0;	// debugging
	//size = getSizeOfState(state);	// debugging
	int i,j;
	
	// you can't just send a struct that has dynamic allocation!!
	
	/*1st parameter is passing the socket we send data on*/
	/*2nd parameter is passing the data we want to send*/
	/*3rd parameter specifies the size of the message*/
	
	// first send the size of the thing
	// bytes_sent = send_data(network_socket, &size, sizeof(long unsigned int));	// debugging
	// then send the basic struct
	bytes_sent += send_data(network_socket,state,sizeof(struct gameState));
//	printf("bytes: %i, ",bytes_sent);
	// then the sprites
	bytes_sent += send_data(network_socket,&(state->allSprites->numSprites),sizeof(int));
//	printf("bytes: %i, ",bytes_sent);

	for (i=0; i<state->allSprites->numSprites; i++) {
		bytes_sent += send_data(network_socket,state->allSprites->spriteArr[i],sizeof(struct sprite));
//printf("sprite #:%i, ", i); fflush(stdout);
//printf("numDisps(%i), ", state->allSprites->spriteArr[i]->numDisps);fflush(stdout);
		for (j=0; j<state->allSprites->spriteArr[i]->numDisps; j++) {
			bytes_sent += send_data(network_socket,state->allSprites->spriteArr[i]->dispArr[j],sizeof(struct dispPair));
		}		
	}
	// then the effects
	bytes_sent += send_data(network_socket,&(state->allEffects->numEffects),sizeof(int));
	for (i=0; i<state->allEffects->numEffects; i++) {
		bytes_sent += send_data(network_socket,state->allEffects->effectArr[i],sizeof(struct effect));
		for (j=0; j<state->allEffects->effectArr[i]->numDisps; j++) {
//printf("disp #:%i, ", j);fflush(stdout);
			bytes_sent += send_data(network_socket,state->allEffects->effectArr[i]->dispArr[j],sizeof(struct dispPair));
		}		
	}
	
	// don't send library, you should never need it

	// no dynamic allocation - this is okay
	bytes_sent += send_data(network_socket,level,sizeof(struct levelData));
	
	return bytes_sent;
	
}

// these were for debugging!
long unsigned getSizeOfState(struct gameState * state) {
	long unsigned int size = 0;
	size = sizeof(struct gameState);
	int i;
	for (i =0; i< state->allSprites->numSprites; i++) {
		size += sizeof(struct dispPair)*state->allSprites->spriteArr[i]->numDisps;
	}
	return size;
}

long unsigned getSizeOfLibrary(struct library * lib) {
	long unsigned int size = 0;
	size = sizeof(struct library);
	int i;
	for (i =0; i< lib->allSprites->numSprites; i++) {
		size += sizeof(struct dispPair)*lib->allSprites->spriteArr[i]->numDisps;
	}
	for (i =0; i< lib->allEffects->numEffects; i++) {
		size += sizeof(struct dispPair)*lib->allEffects->effectArr[i]->numDisps;
	}
	return size;
}


void closing_connection(int network_socket) {
  /* close the socket */
  close(network_socket);
}
