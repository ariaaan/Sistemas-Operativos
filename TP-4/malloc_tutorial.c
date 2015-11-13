#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define META_SIZE sizeof(struct block_meta)

/* Para alinear punteros, todos muliplos de 32 bits */
#define align4(x) (((((x)-1)>>2)<<2)+4)

void *malloc(size_t size);
void free(void *ptr);

/* Tries to find a free block the size of the "size" parameter */
struct block_meta *find_free_block(struct block_meta** last, size_t size);

/* Requests space from the OS using sbrk and adds the block to the list */
struct block_meta *request_space(struct block_meta* last, size_t size);

/* Get the address of our block_meta struct */
struct block_meta *get_block_ptr(void *ptr);

/* Splits a free block in two */
void split_block(struct block_meta* block, size_t size);

/* Prettily prints the linked list of blocks */
void print_memory_list();

struct block_meta {
  size_t size;
  struct block_meta *prev;
  struct block_meta *next;
  int free;
};

void *global_base = NULL;

int main(int argc, char *argv[]){

  void *ptr1 = malloc(140);
  void *ptr2 = malloc(200);
  void *ptr3 = malloc(70);
  void *ptr4 = malloc(80);
  void *ptr5 = malloc(250);

  print_memory_list();
  free(ptr2);
  print_memory_list();
  malloc(80);
  print_memory_list();

	return 0;
}

void *malloc(size_t size) {
  struct block_meta *block;

  size = align4(size);

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found free block
      // TODO: consider splitting block here.

      printf("Minumum size to split: %zu\n", META_SIZE + 4);
      printf("Available Size after Split: %zu\n", block->size - size);

      if((block->size - size) >= META_SIZE + 4) {
        printf("Split Block\n");
        split_block(block, size);
      }

      block->free = 0;
    }
  }

  return(block + 1);
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: consider merging blocks once splitting blocks is implemented.

  struct block_meta* block_ptr = get_block_ptr(ptr);

  block_ptr->free = 1;
}

void split_block(struct block_meta* block, size_t size) {
  size_t total_size = block->size;

  struct block_meta* new = block + META_SIZE + size;

  if(block->next) {
    new->next = block->next;
  }

  block->size = size;
  block->next = new;

  new->size = total_size - size - META_SIZE;
  new->prev = block;
  new->free = 1;
}

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);

  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }

  if (last) { // NULL on first request.
    last->next = block;
  }

  block->size = size;
  block->next = NULL;
  block->prev = last;
  block->free = 0;

  return block;
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*) ptr - 1;
}

void print_memory_list() {
	struct block_meta *b = global_base;

	/* Si la lista esta vacia no hago nada */
	if(b == NULL) return;

	/* Si no esta vacia, la recorro e imprimo los datos */
	int index = 1;

	printf("+----------+----------+----------+------------+------------+------------+\n");
	printf("| %8s | %8s | %8s | %10s | %10s | %10s |\n", "Block Nº", "Size", "Is Free?", "Previous", "This", "Next");
	printf("+----------+----------+----------+------------+------------+------------+\n");

	do {
		printf("| %8d | %8zu | %8s | %10p | %10p | %10p |\n", index, b->size,  (b->free) ? "Yes": "No", b->prev, b, b->next);
		index++;
		b = b->next;
	} while(b != NULL);

	printf("+----------+----------+----------+------------+------------+------------+\n");

}
