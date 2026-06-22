#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void initialize_block(struct header *block, uint64_t size, struct header *next, int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  struct header *current = free_list_ptr;
  while (current != NULL) {
    if (current->size >= size) {
      return current->id;
    }
    current = current->next;
  }
  return -1;
}

int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  uint64_t min_leftover = UINT64_MAX;
  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size) {
      uint64_t leftover = current->size - size;
      if (leftover < min_leftover) {
        min_leftover = leftover;
        best_fit_id = current->id;
      }
    }
    current = current->next;
  }
  return best_fit_id;
}

int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  uint64_t max_leftover = 0;
  int found_any = 0;
  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size) {
      uint64_t leftover = current->size - size;
      if (!found_any || leftover > max_leftover) {
        max_leftover = leftover;
        worst_fit_id = current->id;
        found_any = 1;
      }
    }
    current = current->next;
  }
  return worst_fit_id;
}

int main(void) {
  struct header *free_block1 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block2 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block3 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block4 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block5 = (struct header *)malloc(sizeof(struct header));

  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  printf("First-fit: %d\n", first_fit_id);
  printf("Best-fit: %d\n", best_fit_id);
  printf("Worst-fit: %d\n", worst_fit_id);

  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);

  return 0;
}

/* Pseudocode for coalescing contiguous free blocks
 *
 * coalesce_block(free_list, block_z) {
 *    Traverse free_list and find spot for block_z where linked list stays sorted chronologically by
 *    address location
 *
 *    Let prev_block be free segment right before block_z
 *    Let next_block be free segment right after block_z
 *
 *    Insert block_z into linked list topology chain
 *      prev_block.next = block_z
 *      block_z.next = next_block
 *
 *    Merge evaluation with next_block
 *
 *    Merge evaluation with prev_block
 *
 *    Return modified list head pointer context
 *  }
 */
