#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "interfaces.h"

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
  int port = 2997;
  server_address.sin_port = htons(port);
  /*This is where you specify the IP Address. INADDR_ANY means any adress on the local machine*/
  const char *ipaddr = "128.193.36.41";
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

  /*option to get the clients IP adress here, passed in NULL bc it isn't relevant in this case*/
  while ((client_socket = accept(server_socket, NULL, NULL))) {
    if ((client_socket2 = accept(server_socket, NULL, NULL))) {
      
      struct gameState *state = malloc(sizeof(struct gameState));
      struct library *lib = malloc(sizeof(struct library));
      struct levelData *level = malloc(sizeof(struct levelData));

      int input = ' ';

      while (input != 'q') {
        /*1st parameter is passing the socket we send data on*/
        /*2nd parameter is passing the data we want to send*/
        /*3rd parameter specifies the size of the message*/
        send(client_socket, state, sizeof(struct gameState), 0);
        send(client_socket2, state, sizeof(struct gameState), 0);
        send(client_socket, lib, sizeof(struct library), 0);
        send(client_socket2, lib, sizeof(struct library), 0);
        send(client_socket, level, sizeof(struct levelData), 0);
        send(client_socket2, level, sizeof(struct levelData), 0);

        recv(client_socket, &input, sizeof(int), 0);
        printf("input: %d\n", input);
        recv(client_socket2, &input, sizeof(int), 0);
        printf("input: %d\n", input);
      }
      if (input == 'q') {
        break;
      }
    }
  }


  /*finished basic server app, time to close the socket*/
  close(server_socket);

  return 0;
}
