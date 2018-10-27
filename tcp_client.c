#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int network_socket;

int establish_connection() {
  network_socket = socket(AF_INET, SOCK_STREAM, 0);
  /* specify the address for the socket */
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  /* specify port 10005 */
  server_address.sin_port = htons(2997);
  /* ip address connection (server_address) */
  server_address.sin_addr.s_addr = inet_addr("128.193.36.41");

  /* calling the actual connect function here */
  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

  return connection_status;
}

char* receive_data() {
  /* this string holds the information we get back from the server */
  static char server_response[256];

  /* receive data from the server */
  recv(network_socket, &server_response, sizeof(server_response), 0);

  return server_response;
}

void closing_connection() {
  /* close the socket */
  close(network_socket);
}
