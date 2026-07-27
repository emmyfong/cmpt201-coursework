/*
  1. What is the address of the server it is trying to connect to (IP address and port number).
  IP address is 127.0.0.1 (local host) and port is 8000

  2. Is it UDP or TCP? How do you know?
  TCP because the socket is created using SOCK_STREAM

  3. The client is going to send some data to the server. Where does it get this data from? HOw can
  you tell in the code?
  It gets data from standard input (keyboard) and we can tell because the code calls
  read(STDIN_FILENO, buf, BUF_SIZE) in the while loop

  4. How does the client program end? How can you tell that in the code?
  Ends when user gives an empty input or EOF (ctrl d) causing the read function to return a value <=
  1 which breaks the while loop and exits
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;

  // TODO: print the message received from client
  // TODO: increase total_message_count per message
  int cfd = client->cfd;
  int client_id = client->client_id;
  free(client);

  char buf[BUF_SIZE];
  ssize_t num_read;

  while ((num_read = read(cfd, buf, BUF_SIZE - 1)) > 0) {
    buf[num_read] = '\0';

    if (buf[num_read - 1] == '\n') {
      buf[num_read - 1] = '\0';
    }

    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    int current_count = total_message_count;
    pthread_mutex_unlock(&count_mutex);

    printf("Msg # %3d; Clinet ID %d: %s\n", current_count, client_id, buf);
  }

  if (num_read == -1) {
    perror("read");
  }

  // When read() returns <= 0 -> client is disconnected
  printf("Ending thread for client %d\n", client_id);
  close(cfd);

  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }

  for (;;) {
    // TODO: create a new thread when a new connection is encountered
    // TODO: call handle_client() when launching a new thread, and provide
    // client_info

    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);

    int cfd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_len);
    if (cfd == -1) {
      handle_error("accept");
    }

    pthread_mutex_lock(&client_id_mutex);
    int assigned_id = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    struct client_info *info = malloc(sizeof(struct client_info));
    if (info == NULL) {
      handle_error("malloc");
    }

    info->cfd = cfd;
    info->client_id = assigned_id;

    printf("New client created -> ID %d on socket FD %d\n", assigned_id, cfd);

    pthread_t thread_id;
    int s = pthread_create(&thread_id, NULL, handle_client, info);
    if (s != 0) {
      handle_error("pthread_create");
    }

    s = pthread_detach(thread_id);
    if (s != 0) {
      handle_error("pthread_detach");
    }
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}
