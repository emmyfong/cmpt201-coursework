#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char *buffer = NULL;
  size_t buffSize = 0;

  while (1) {
    printf("Enter programs to run.\n");
    ssize_t numChars = getline(&buffer, &buffSize, stdin);

    // Check to see if enter was pressed or player typed exit
    if (buffer[0] == '\n' || strcmp(buffer, "exit\n") == 0) {
      break;
    }

    // If there is no input
    if (numChars == -1) {
      break;
    }

    // Get rid of the trailing \n
    if (numChars > 0 && buffer[numChars - 1] == '\n') {
      buffer[numChars - 1] = '\0';
    }

    char *org_path = strdup(buffer);

    // Tokenization
    char *ptr;
    // Use / as deliminator
    char *token = strtok_r(buffer, "/", &ptr);
    char *program = NULL;

    // Loop through path to get last command
    while (token != NULL) {
      program = token;
      token = strtok_r(NULL, "/", &ptr);
    }

    // Fork new process
    pid_t pid = fork();

    if (pid == 0) {
      // Child process
      execl(org_path, program, NULL);

      perror("Exec failed");
      free(org_path);
      free(buffer);
      exit(EXIT_FAILURE);

    } else {
      // Parent process
      int status;
      waitpid(pid, &status, 0);
      free(org_path);
    }
  }

  free(buffer);
  return 0;
}
