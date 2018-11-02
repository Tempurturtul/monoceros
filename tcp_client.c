#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "interfaces.h"

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

  bytes_received += recv(network_socket, state, sizeof(struct gameState), 0);
  bytes_received += recv(network_socket, lib, sizeof(struct library), 0);
  bytes_received += recv(network_socket, level, sizeof(struct levelData), 0);

  return bytes_received;
}

int send_data(int network_socket, void *payload, int payload_len) {
  int bytes_sent = send(network_socket, payload, payload_len, 0);

  return bytes_sent;
}

void closing_connection(int network_socket) {
  /* close the socket */
  close(network_socket);
}
