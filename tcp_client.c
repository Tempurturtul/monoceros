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
	int bytes_received = 0;
	long unsigned int packageSize=0;
	int i,j;
	
//	printf("inside recv\n");
//	freeSpriteList(state->allSprites);
//	freeEffectList(state->allEffects);
//	printf("freed lists\n");
	
	
	state->allSprites = malloc(sizeof(struct spriteList));
	bytes_received += recv(network_socket, &packageSize, sizeof(long unsigned int), 0);
//	printf("bytes recv: %i\n", bytes_received); sleep(1);
	bytes_received += recv(network_socket, state, sizeof(struct gameState), 0);
//	printf("bytes recv: %i\n", bytes_received); sleep(1);
	bytes_received += recv(network_socket, &(state->allSprites->numSprites), sizeof(int), 0);
//	printf("bytes recv: %i\n", bytes_received); sleep(1);
	for (i=0; i<state->allSprites->numSprites; i++) {
		state->allSprites->spriteArr[i] = malloc(sizeof(struct sprite));
		bytes_received += recv(network_socket, state->allSprites->spriteArr[i], sizeof(struct sprite), 0);
		for (j=0; j<state->allSprites->spriteArr[i]->numDisps; j++) {
			state->allSprites->spriteArr[i]->dispArr[j] = malloc(sizeof(struct dispPair));
			bytes_received += recv(network_socket, state->allSprites->spriteArr[i]->dispArr[j], sizeof(struct dispPair), 0);
		}
	}
	state->allEffects = malloc(sizeof(struct effectList));
	bytes_received += recv(network_socket, &(state->allEffects->numEffects), sizeof(int), 0);
//	printf("bytes recv: %i\n", bytes_received); sleep(1);
	for (i=0; i<state->allEffects->numEffects; i++) {
		state->allEffects->effectArr[i] = malloc(sizeof(struct effect));
		bytes_received += recv(network_socket, state->allEffects->effectArr[i], sizeof(struct effect), 0);
		for (j=0; j<state->allEffects->effectArr[i]->numDisps; j++) {
			state->allEffects->effectArr[i]->dispArr[j] = malloc(sizeof(struct dispPair));
			bytes_received += recv(network_socket, state->allEffects->effectArr[i]->dispArr[j], sizeof(struct dispPair), 0);
		}
	}
	
//	printf("state complete\n");	
/*
	while (bytes_received < packageSize+sizeof(long unsigned int)) {
		struct sprite * temp = malloc(sizeof(struct sprite));
		bytes_received += recv(network_socket, state, packageSize, 0);
		printf("bytes recv: %i\n", bytes_received); sleep(1);		
	}
*/
	
//	bytes_received += recv(network_socket, &packageSize, sizeof(long unsigned int), 0);
//	bytes_received += recv(network_socket, lib, packageSize, 0);

	bytes_received += recv(network_socket, level, sizeof(struct levelData), 0);
//	printf("level complete\n");	
	return bytes_received;
}

int send_data(int network_socket, void *payload, int payload_len) {
  int bytes_sent = send(network_socket, payload, payload_len, 0);

  return bytes_sent;
}

int send_all(int network_socket, struct gameState *state, struct library *lib, struct levelData *level) {
	int bytes_sent=0;
	long unsigned int size = 0;
	int i,j;
	
	size = getSizeOfState(state);
	// first send the size of the thing
	bytes_sent = send_data(network_socket, &size, sizeof(long unsigned int));
	// then send the basic struct
	bytes_sent += send_data(network_socket,state,sizeof(struct gameState));
	// then the sprites
	bytes_sent += send_data(network_socket,&(state->allSprites->numSprites),sizeof(int));
	for (i=0; i<state->allSprites->numSprites; i++) {
		bytes_sent += send_data(network_socket,state->allSprites->spriteArr[i],sizeof(struct sprite));
		for (j=0; j<state->allSprites->spriteArr[i]->numDisps; j++) {
			bytes_sent += send_data(network_socket,state->allSprites->spriteArr[i]->dispArr[j],sizeof(struct dispPair));
		}		
	}
	// then the effects
	bytes_sent += send_data(network_socket,&(state->allEffects->numEffects),sizeof(int));
	for (i=0; i<state->allEffects->numEffects; i++) {
		bytes_sent += send_data(network_socket,state->allEffects->effectArr[i],sizeof(struct effect));
		for (j=0; j<state->allEffects->effectArr[i]->numDisps; j++) {
			bytes_sent += send_data(network_socket,state->allEffects->effectArr[i]->dispArr[j],sizeof(struct dispPair));
		}		
	}
	
	
	// don't send library, you should never need it
//	size = getSizeOfLibrary(lib);
//	bytes_sent += send_data(network_socket,&size,sizeof(long unsigned int));
//	bytes_sent += send_data(network_socket,lib,size);
	// no dynamic allocation - this is okay
	bytes_sent += send_data(network_socket,level,sizeof(struct levelData));
	
	return bytes_sent;
	
}
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
