#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main () {
  /* create the socket */
  int network_socket;
  /* specifying TCP protocol here */
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

  /* check for errors in the connection */
  if (connection_status == -1) {
    printf("There was an error making a connection to the remote socket!\n\n");
  }

  /* this string holds the information we get back from the server */
  char server_response[256];
  /* receive data from the server */
  recv(network_socket, &server_response, sizeof(server_response), 0);

  /* show the user the data we received from the server */
  printf("The server sent the response: %s\n", server_response);

  /* close the socket */
  close(network_socket);

  return 0;
}
