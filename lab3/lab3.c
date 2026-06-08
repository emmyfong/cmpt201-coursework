#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char *history[5] = {NULL, NULL, NULL, NULL, NULL};
  int count = 0;

  // Variables for getline
  char *buffer = NULL;
  size_t buffSize = 0;

  while (1) {
    printf("Enter input: ");

    ssize_t numChars = getline(&buffer, &buffSize, stdin);
    if (numChars == -1) {
      // Exit loop
      break;
    }

    if (buffer[0] == '\n') {
      printf("that was blank!\n");
      continue;
    }

    // Check to see if the current index is still in 5
    // Duplicate the buffer into history array and increment
    int index = count % 5;

    if (history[index] != NULL) {
      free(history[index]);
    }

    history[index] = strdup(buffer);
    count++;

    // check if user typed print\n
    if (strcmp(buffer, "print\n") == 0) {
      int start_idx = count % 5;

      for (int i = 0; i < 5; i++) {
        int read_idx = (start_idx + i) % 5;
        if (history[read_idx] != NULL) {
          printf("[%d]: %s", read_idx, history[read_idx]);
        }
      }
    }
  }

  for (int i = 0; i < 5; i++) {
    if (history[i] != NULL) {
      free(history[i]);
    }
  }

  free(buffer);
  return 0;
}
