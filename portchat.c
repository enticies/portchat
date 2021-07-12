#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <ncurses.h>

#define ANSI_COLOR_GREEN   "\e[0;32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void menu();
void createServer();
void conServer(char *ip, int);
void * getInput(void * arg);
void usage();
void coloring();
int checkPort(char *);

char input[99999];
int sendFlag = 0;

int input_length;

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
  char server_message[] = "You have connected to the server!\n\n";

  pthread_t thread1;
  pthread_create(&thread1, NULL, getInput, NULL);

  // create the server socket
  long server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);


  // define the server address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  // bind the socket to our specified IP and port
  int server_status = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if (server_status == 0) {
    initscr();
    /* start_color(); */
    /* assume_default_colors(COLOR_BLACK, COLOR_WHITE); */
    /* init_pair(1, COLOR_GREEN, COLOR_BLACK); */
    /* init_pair(2, COLOR_BLACK, COLOR_WHITE); */
    /* attrset(COLOR_PAIR(1) | A_BOLD); */
    printf("Server has been successfully created!\t\t\tPort: %d\n\n", port);
    refresh();
  }
  else {
    printError("server creation");
    exit(1);
  }

  listen(server_socket, 1);

  int client_socket;
  client_socket = accept(server_socket, NULL, NULL);

  if (client_socket > -1) {
    /* attrset(COLOR_PAIR(1) | A_BOLD); */
    printf("Someone has connected to the server!");
    pthread_create(&thread1, NULL, getInput, NULL); // don't need to create every loop
    /* attroff(COLOR_PAIR(1)); */
    /* refresh(); */
    send(client_socket, server_message, sizeof(server_message), MSG_DONTWAIT);
  }
  int row, col;
  getmaxyx(stdscr, row, col);
  int col_inc = 4; 


  pthread_create(&thread1, NULL, getInput, NULL);
  while (client_socket > -1) {
    char client_response[99999] = "\n";
    refresh();
    /* attrset(COLOR_PAIR(2)); */

    int length = recv(client_socket, &client_response, sizeof(client_response), MSG_DONTWAIT);

    if (length == 0) {
      printError("closed connection");
      break;
    }

    if (sendFlag == 1) {
      send(client_socket, input, sizeof(input), 0);
      sendFlag = 0;
      pthread_create(&thread1, NULL, getInput, NULL);
    }
    client_response[length] = '\0';
    if (length > 0 && strcmp(client_response, "\n") != 0) {
      /* printf("%*c%s\n", ' ', 50, client_response); */
      mvprintw(col-(col-col_inc), row/2, "Other Side: %s", client_response);
      col_inc++;
    }
  }

  // close the socket
  close(server_socket);
  // end ncurses
  endwin();
  printError("closed connection");
}

void conServer(char * ip, int port) {
  // create a socket
  long network_socket;

  pthread_t thread1;
  pthread_create(&thread1, NULL, getInput, NULL);

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
    initscr();
    /* start_color(); */
    /* assume_default_colors(COLOR_BLACK, COLOR_WHITE); */
    /* init_pair(1, COLOR_GREEN, COLOR_BLACK); */
    /* init_pair(2, COLOR_BLACK, COLOR_WHITE); */
    /* attrset(COLOR_PAIR(1) | A_BOLD); */
    addstr("Connection to the server established.\n");
 }


  int row, col;
  getmaxyx(stdscr, row, col);
  int col_inc = 4; 

  pthread_create(&thread1, NULL, getInput, NULL);
  while (connection_status == 0) {
    refresh();
    char server_response[99999] = "\n";
    attrset(COLOR_PAIR(2));
    int length = recv(network_socket, &server_response, sizeof(server_response), MSG_DONTWAIT);

    if (length == 0) {
      endwin();
      printError("closed connection");
      return;
    }

    if (sendFlag == 1 && strcmp(input, "\n") != 0) {
      send(network_socket, input, sizeof(input), 0);
      sendFlag = 0;
      pthread_create(&thread1, NULL, getInput, NULL);
    }
    if (length > 0 && strcmp(server_response, "\n") != 0) {
      mvprintw(col-(col-col_inc), row/2, "Other Side: %s\n", server_response);
      col_inc++;
  }
  }
  endwin();
  printError("closed connection");
}

void* getInput(void * arg) {
  int ch;
  int index = 0;
  cbreak();  
  while ((ch = getch()) != '\n') {
    input[index] = ch;
    index++;
  }
  input[index] = '\0';
  printf("\n");
  refresh();
  sendFlag = 1;
  return NULL;
}

void printError(char * errorType) {
  // input related
  if (strcmp(errorType, "large input") == 0) {
    printf(ANSI_COLOR_RED "Input too large.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "usage") == 0) {
    printf("Incorrect usage. Use the -h flag to see the correct usage.\n");
  }
  else if (strcmp(errorType, "incorrect input") == 0) {
    printf(ANSI_COLOR_RED "\nIncorrect input.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "incorrect port") == 0) {
    printf(ANSI_COLOR_RED "\nIncorrect port number.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "range error") == 0) {
    printf(ANSI_COLOR_RED "\nNumber must be in range 0-63335.\n" ANSI_COLOR_RESET "\n");
  }
  // connection related
  else if (strcmp(errorType, "closed connection") == 0) {
    printf(ANSI_COLOR_RED "\nConnection closed.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "server creation") == 0) {
    printf(ANSI_COLOR_RED "\nUnable to create the server.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "not connected") == 0) {
    printf(ANSI_COLOR_RED "\nCouldn't connect to the server.\n" ANSI_COLOR_RESET "\n");
  }
  else if (strcmp(errorType, "ip") == 0) {
    printf(ANSI_COLOR_RED "\nInvalid IP address.\n" ANSI_COLOR_RESET "\n");
  }
}
