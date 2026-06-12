#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024

struct header {
  uint64_t size;
  struct header *next;
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len;

  if (data_size == sizeof(uint64_t)) {
    len = snprintf(buf, BUF_SIZE, format, *(uint64_t *)data);
  } else {
    len = snprintf(buf, BUF_SIZE, format, *(void **)data);
  }

  if (len < 0) {
    perror("snprintf");
    exit(EXIT_FAILURE);
  }

  write(STDOUT_FILENO, buf, len);
}

int main(void) {
  void *heap_start = sbrk(256);
  if (heap_start == (void *)-1) {
    perror("sbrk failed");
    return 1;
  }

  // pointer set up for block structs
  struct header *block1 = (struct header *)heap_start;
  struct header *block2 = (struct header *)((char *)heap_start + 128);

  block1->size = 128;
  block1->next = NULL;

  block2->size = 128;
  block2->next = block1;

  // print headers before filling data
  print_out("first block:       %p\n", &block1, sizeof(&block1));
  print_out("second block:      %p\n", &block2, sizeof(&block2));
  print_out("first block size:  %lu\n", &block1->size, sizeof(block1->size));
  print_out("first block next:  %p\n", &block1->next, sizeof(block1->next));
  print_out("second block size: %lu\n", &block2->size, sizeof(block2->size));
  print_out("second block next: %p\n", &block2->next, sizeof(block2->next));

  // where data region start
  size_t header_size = sizeof(struct header);
  size_t data_size = 128 - header_size;

  char *block1_data = (char *)block1 + header_size;
  char *block2_data = (char *)block2 + header_size;

  memset(block1_data, 0, data_size);
  memset(block2_data, 1, data_size);

  for (size_t i = 0; i < data_size; i++) {
    char val = block1_data[i] + '0';
    write(STDOUT_FILENO, &val, 1);
    write(STDOUT_FILENO, "\n", 1);
  }

  return 0;
}
