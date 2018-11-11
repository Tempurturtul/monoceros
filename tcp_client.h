#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "interfaces.h"

int establish_connection(const char *ip_address, int port);
int receive_data(int network_socket, struct gameState *state, struct library *lib, struct levelData *level);
int send_data(int network_socket, void *payload, int payload_len);
void closing_connection(int network_socket);

int send_all(int network_socket, struct gameState *state, struct library *lib, struct levelData *level);
long unsigned getSizeOfState(struct gameState * state);
long unsigned getSizeOfLibrary(struct library * lib);
long unsigned getSizeOfLevel(struct levelData * level);

#endif
