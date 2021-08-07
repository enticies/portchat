#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define ANSI_COLOR_GREEN   "\e[0;32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void menu();
void createServer(int);
void conServer(char *ip, int);
void * getInput(void * arg);
void usage();
void coloring();
int checkPort(char *);
void print_error(int);

char input[99999];
int sendFlag = 0;

int input_length;

int main(int argc, char **argv) {
  if (argc < 2) {
    print_error(1);
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
    print_error(4);
  }
  else {
    print_error(3);
  }
  exit(1);
}

void createServer(int port) {
  pthread_t thread1;
  struct winsize w;
  int columns = w.ws_col;
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
    printf("Server has been successfully created!\t\t\tPort: %d\n\n", port);
  }
  else {
    print_error(6);
    exit(1);
  }

  listen(server_socket, 1);

  int client_socket;
  client_socket = accept(server_socket, NULL, NULL);

  if (client_socket > -1) {
    printf("\n\nSomeone has connected to the server!\n\n");
    int ch;
    while ((ch = getchar()) != '\n') { }   // flush the input buffer
    pthread_create(&thread1, NULL, getInput, NULL); // don't need to create every loop
  }

  while (client_socket > -1) {
    ioctl(0, TIOCGWINSZ, &w);
    columns = w.ws_col;
    char client_response[99999] = "\n";

    int length = recv(client_socket, &client_response, sizeof(client_response), MSG_DONTWAIT);

    if (length == 0) {
      print_error(5);
      exit(0);
      break;
    }

    if (sendFlag == 1) {
      send(client_socket, input, strlen(input), 0);
      sendFlag = 0;
      pthread_create(&thread1, NULL, getInput, NULL);
    }
    client_response[length] = '\0';
    if (length > 0 && strcmp(client_response, "\n") != 0) {
      printf("%*sOther Side: %s\n", (int) ((float) columns * 0.5),  " ", client_response);
    }
  }

  // close the socket
  close(server_socket);
  // end ncurses
  print_error(5);
}

void conServer(char * ip, int port) {
  struct winsize w;
  int columns = w.ws_col;
  // create a socket
  long network_socket;

  pthread_t thread1;

  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  // specify an address for the socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (inet_pton(AF_INET, ip, &(server_address.sin_addr)) < 1) {
    print_error(8);
    exit(1);
  }

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  // check for error with the connection
  if (connection_status == -1) {
    print_error(7);
    return;
  }
  else {
    printf("Connection to the server established.\n");
    pthread_create(&thread1, NULL, getInput, NULL);
 }

  while (connection_status == 0) {
    ioctl(0, TIOCGWINSZ, &w);
    columns = w.ws_col;
    char server_response[99999] = "\n";
    int length = recv(network_socket, &server_response, sizeof(server_response), MSG_DONTWAIT);

    if (length == 0) {
      print_error(5);
      return;
    }

    if (sendFlag == 1) {
      send(network_socket, input, strlen(input), 0);
      sendFlag = 0;
      pthread_create(&thread1, NULL, getInput, NULL);
    }
    server_response[length] = '\0';
    if (length > 0 && strcmp(server_response, "\n") != 0) {
      printf("%*sOther Side: %s\n", (int) ((float) columns * 0.5),  " ", server_response);
  }
  }
  print_error(5);
}

void* getInput(void * arg) {
  int ch;
  if (strlen(fgets(input, sizeof(input), stdin)) < 100000) {
    input[strlen(input)-1] = '\0';
    sendFlag = 1;
  }
  else {
    print_error(0);
  }
  return NULL;
}

void print_error(int error_code) {
  char * errors[] = {
    "Input too large.",
    "Incorrect usage. Use the -h flag  to see the correct usage",
    "Incorrect input.",
    "Incorrect port number.",
    "Number must be in range 0-63335.",
    "Connect close.",
    "Unable to create a server.", 
    "Couldn't connect to the server.",
    "Invalid IP address."
  };

  printf(ANSI_COLOR_RED "%s\n", errors[error_code], ANSI_COLOR_RESET "\n");

}
