#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int establish_connection(const char *ip_address, int port);
int receive_data(int network_socket, char *buffer, int buffer_len);
void closing_connection(int network_socket);

#endif
