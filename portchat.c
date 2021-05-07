#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void menu();
void listenPort();
void flushBuffer();
int getPort();


int main() {
  printf("Welcome to portchatter.\n\nPlease pick an option:\n");
  menu();
  return 0;
}

void menu() {
  int choice;
  int port;

  while (1) {
    printf("%s", 
    "1: Listen on a port\n"
    "2: Connect to a port\n"
    "3: Exit\n");

    scanf("%d", &choice); // vulnerable to buffer overflow 
    while (getchar() != '\n') { } 
    if (choice == 1) {
      port = getPort();
      listenPort(port);
    }
    else if (choice == 2) {
      port = getPort();
      conPort(port);
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
    printf("Please enter a port number:\n");
    fflush(stdin);
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
      printf(ANSI_COLOR_RED "\nIncorrect input.\n" ANSI_COLOR_RESET "\n");
      flag = 1;
    }
  }
}

void listenPort(int port) {
  char server_message[256] = "You have reached the server!";
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
  
  listen(server_socket, 1);

  int client_socket;

  client_socket = accept(server_socket, NULL, NULL);
  while (client_socket != -1) {
    send(client_socket, server_message, sizeof(server_message), 0);
  }
}

void conPort(int port) {
  int n_socket;
  char server_response[256];

  n_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int connection_status = connect(n_socket, (struct sockaddr *) &server_address, sizeof(server_address));

  if (connection_status == -1) {
    printf(ANSI_COLOR_RED "\nCouldn't connect to the port.\n" ANSI_COLOR_RESET "\n");
    return;
  }
  // receive data from the port
  int status;
  recv(n_socket, &server_response, sizeof(server_response), 0);


}

