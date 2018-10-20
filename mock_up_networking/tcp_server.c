#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main() {
  /*create string to hold the data sent to the clients*/
  char server_message[256] = "You have reached the server. Congratulations, kid.";

  /*hold return value of our socket call*/
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  /*Define address structure*/
  struct sockaddr_in server_address;

  /*specifies the connection family*/
  server_address.sin_family = AF_INET;
  /*specifies the listening port*/
  server_address.sin_port = htons(2997);
  /*This is where you specify the IP Address. INADDR_ANY means any adress on the local machine*/
  server_address.sin_addr.s_addr = inet_addr("128.193.36.41");

  /*binding the socket to our specified IP and port*/
  bind(server_socket,(struct sockaddr*) &server_address, sizeof(server_address));

  /*use the listen function to start listening to connections*/
  /*number corresponds to how many connections we want to have*/
  listen(server_socket, 2);

  /*holds client socket*/
  int client_socket;
  /*option to get the clients IP adress here, passed in NULL bc it isn't relevant in this case*/
  client_socket = accept(server_socket, NULL, NULL);

  /*1st parameter is passing the socket we send data on*/
  /*2nd parameter is passing the data we want to send*/
  /*3rd parameter specifies the size of the message*/
  send(client_socket, server_message, sizeof(server_message), 0);

  /*finished basic server app, time to close the socket*/
  close(server_socket);

  return 0;
}
