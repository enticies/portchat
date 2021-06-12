#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\e[0;32m"

void menu();
void createServer();
void flushBuffer();
int getPort();


int main() {
  printf("Welcome to portchat.\n\nPlease pick an option:\n");
  menu();
  return 0;
}

void menu() {
  int choice;
  int port;

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
  }
}

void createServer(int port) {
  char server_message[] = "You have connected to the server!\n\n";
  // create the server socket
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  // bind the socket to our specified IP and port
  bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));



  }

void conServer(int port) {
  // create a socket
  int network_socket;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  // specify an address for the socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  // checkk for error with the connection
  if (connection_status == -1) {
    printf(ANSI_COLOR_RED "\nCouldn't connect to the server.\n" ANSI_COLOR_RESET "\n");
    return;
  }
  else {
    printf(ANSI_COLOR_GREEN "\nConnection to the server established.\n\n" ANSI_COLOR_RESET "\n");
  }
  
  while (1) {
    char server_response[256];
    int length = recv(network_socket, &server_response, sizeof(server_response), 0);
    server_response[length] = '\0';
    if (strcmp(server_response, "\n") != 0) {
      printf("From server: %s", server_response);
    }
  }


}

