#ifndef PRINTERROR
#define PRINTERROR

#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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
  else if (strcmp(errorType, "wrong option") == 0) {
    printf(ANSI_COLOR_RED "\nPlease pick one of the options.\n" ANSI_COLOR_RESET "\n");
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

#endif
