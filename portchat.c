#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include "printerror.h"

#define ANSI_COLOR_GREEN   "\e[0;32m"

void menu();
void createServer();
void conServer(char *ip, int);
void * sendInput(void * arg);
void usage();
int checkPort(char *);

char input[99999];
int sendFlag = 0;

int main(int argc, char **argv) {
  if (argc < 2) {
    printError("usage");
  }
  else { 
    if (strcmp(argv[1], "-h") == 0) {
      usage();
      exit(0);
    }
    else if ((strcmp(argv[1], "-c")) == 0 && argc == 4) {
      int port = checkPort(argv[3]);
      conServer(argv[2], port);
    }
    else if ((strcmp(argv[1], "-s")) == 0 && argc == 3) {
      int port = checkPort(argv[2]);
      createServer(port);
    }
    else {
      usage();
      exit(1);
    }
  return 0;
}
}

void usage() {
  printf("create a server: portchat -s [port]\n"
         "connect to somewhere: portchat -c IP port\n"
         "options:\n"
                  "-c        connect to some server by supplying the IP and the port\n"
                  "-s        create a server on the local machine\n"
                  "-h        print this help message\n");
         }
           

int checkPort(char * port) {
  int flag = 1;
  int int_port;

  for (int i=0; i<strlen(port); i++) {
    if (!isdigit(port[i])) {
      flag = 0;
    }
  }
  if (flag) {
    int_port = atoi(port);
    if (int_port >= 0 && int_port <= 65535) {
      return int_port;
    }
    printError("range error");
  }
  else {
    printError("incorrect port");
  }
  exit(1);
}

void createServer(int port) {
  char server_message[] = "You have connected to the server!\n";

  pthread_t thread1;
  pthread_create(&thread1, NULL, sendInput, NULL);

  // create the server socket
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  // bind the socket to our specified IP and port
  int server_status = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if (server_status == 0) {
    printf(ANSI_COLOR_GREEN "\nServer has been successfully created!\n" ANSI_COLOR_RESET "\n");
  }
  else {
    printError("server creation");
  }

  listen(server_socket, 1);

  int client_socket;
  client_socket = accept(server_socket, NULL, NULL);

  if (client_socket > -1) {
    printf(ANSI_COLOR_GREEN "\nSomeone has connected to the server!\n" ANSI_COLOR_RESET "\n");
    send(client_socket, server_message, sizeof(server_message), MSG_DONTWAIT);

  }

  while (client_socket > -1) {
    char client_response[99999] = "\n";
    int length = recv(client_socket, &client_response, sizeof(client_response), MSG_DONTWAIT);

    if (length == 0) {
      printError("closed connection");
      break;
    }
    if (sendFlag == 1) {
      send(client_socket, input, sizeof(input), 0);
      sendFlag = 0;
    }
    /* client_response[length] = '\0'; */
    if (strcmp(client_response, "\n") != 0) {
      printf("%*c%s", 50, ' ', client_response);
    }
    pthread_create(&thread1, NULL, sendInput, NULL);
  }

  // close the socket
  close(server_socket);
}

void conServer(char * ip, int port) {
  // create a socket
  int network_socket;

  pthread_t thread1;
  pthread_create(&thread1, NULL, sendInput, NULL);

  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  // specify an address for the socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  if (inet_pton(AF_INET, ip, &(server_address.sin_addr)) < 1) {
    printError("ip");
    exit(1);
  }

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  // check for error with the connection
  if (connection_status == -1) {
    printError("not connected");
    return;
  }
  else {
    printf(ANSI_COLOR_GREEN "\nConnection to the server established.\n" ANSI_COLOR_RESET "\n");
 }

  while (connection_status == 0) {
    char server_response[99999] = "\n";
    int length = recv(network_socket, &server_response, sizeof(server_response), MSG_DONTWAIT);

    if (length == 0) {
      printError("closed connection");
      return;
    }
    if (sendFlag == 1) {
      send(network_socket, input, sizeof(input), 0);
      sendFlag = 0;
    }
    /* server_response[length] = '\0'; */
    if (strcmp(server_response, "\n") != 0) {
      printf("%*c%s", 50, ' ', server_response);
  }
    pthread_create(&thread1, NULL, sendInput, NULL);
  }
}

void* sendInput(void * arg) {
  if (strlen(fgets(input, sizeof(input), stdin)) < 100000) {
    sendFlag = 1;
  }
  else {
    printError("large input");
  }
  return NULL;
}

