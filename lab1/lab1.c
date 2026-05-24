#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *buffer = NULL;
  size_t buffSize = 0;

  while (1) {

    printf("Please enter some text: ");
    ssize_t numChars = getline(&buffer, &buffSize, stdin);

    if (numChars == 1 && buffer[0] == '\n') {
      break;
    }

    // Split string into tokens
    //  Pointer to save progress
    char *ptr;
    char *token = strtok_r(buffer, " \n", &ptr);

    // grab tokens until strtok_r returns NULL
    while (token != NULL) {
      printf("  %s\n", token);
      token = strtok_r(NULL, " \n", &ptr);
    }
  }

  free(buffer);
  return 0;
}
