#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\e[0;32m"

void menu();
void createServer();
int getPort();
void * sendInput(void * arg);

char input[256];
int sendFlag = 0;

int main() {
  
  printf("Welcome to portchat.\n\nPlease pick an option:\n");
  menu();
  return 0;
}

void menu() {
  int choice;
  int port;

  pthread_t thread1;

  while (1) {
    printf("%s", 
    "1: Create a server\n"
    "2: Connect to a server\n"
    "3: Exit\n");

    scanf("%d", &choice); // vulnerable to buffer overflow 

    while (getchar() != '\n') { } // clear the buffer

    if (choice == 1) {
      port = getPort();
      createServer(port);
    }
    else if (choice == 2) {
      port = getPort();
      conServer(port);
    }
    else if (choice == 3) {
      printf("Bye.\n");
      exit(0);
    }
    else {
      printf(ANSI_COLOR_RED "\nPlease pick one of the options.\n" ANSI_COLOR_RESET "\n");
    }
    fflush(stdin);
  }
}

int getPort() {
  int port;
  char portString[20];
  int flag = 1;

  while (1) {
    printf("\nPlease enter the address of the server:\n"); // for now only the port number
    fflush(stdin);                // clear the buffer
    fgets(portString, 20, stdin);

    if (strcmp(portString, "q") == 0) {
      break;
    }

    for (int i=0; i<strlen(portString)-1; i++) {
      if (!isdigit(portString[i])) {
        flag = 0;
      }
    }
    
    if (flag) {
      port = atoi(portString);
      if (port >= 0 && port <= 65535) {
        return port;
      }
      printf(ANSI_COLOR_RED "\nNumber must be in range 0-63335.\n" ANSI_COLOR_RESET "\n");
    }
    else {
      printf(ANSI_COLOR_RED "\nIncorrect input." ANSI_COLOR_RESET "\n");
      flag = 1;
    }
    fflush(stdin);
  }
}

void createServer(int port) {
  char server_message[] = "You have connected to the server!\n";
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
    printf(ANSI_COLOR_RED "\nUnable to create the server.\n" ANSI_COLOR_RESET "\n");
  }

  listen(server_socket, 1);

  int client_socket;
  client_socket = accept(server_socket, NULL, NULL);

  if (client_socket > -1) {
    printf(ANSI_COLOR_GREEN "\nSomeone has connected to the server!\n" ANSI_COLOR_RESET "\n");
    send(client_socket, server_message, sizeof(server_message), 0);
  }

  while (client_socket > -1) {
    char response[256];
    int length = recv(client_socket, &response, sizeof(response), 0);
    if (length = 0) {
      printf(ANSI_COLOR_RED "\nConnection closed.\n" ANSI_COLOR_RESET "\n");
      break;
    }
    response[length] = '\0';
    if (strcmp(response, "\n") != 0) {
      printf("");
    }
  
  
  // close the socket
  close(server_socket);
  }
}

void conServer(int port) {
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

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  // check for error with the connection
  if (connection_status == -1) {
    printf(ANSI_COLOR_RED "\nCouldn't connect to the server.\n" ANSI_COLOR_RESET "\n");
    return;
  }
  else {
    printf(ANSI_COLOR_GREEN "\nConnection to the server established.\n" ANSI_COLOR_RESET "\n");
 }

  while (connection_status == 0) {
    char server_response[256] = "\n";
    int length = recv(network_socket, &server_response, sizeof(server_response), MSG_DONTWAIT);
    if (length == 0) {
      printf(ANSI_COLOR_RED "\nConnection closed.\n" ANSI_COLOR_RESET "\n");
      break;
    }
    if (sendFlag == 1) {
      send(network_socket, input, sizeof(input), 0);
      sendFlag = 0;
    }
    server_response[length] = '\0';

    if (strcmp(server_response, "\n") != 0) {
      printf("%*c%s", 30, ' ', server_response);
  }
    pthread_create(&thread1, NULL, sendInput, NULL);
  }
    printf("Connection closed.\n");
}

void* sendInput(void * arg) {
  fgets(input, sizeof(input), stdin);
  sendFlag = 1;
  return NULL;
}

    



