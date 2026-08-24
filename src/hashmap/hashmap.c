#include "utils/delete_from_ptr.h"
#include "hashmap/hashmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

size_t hash_to_djb2(unsigned char *str) {
  unsigned long hash = 5381;
  for (int i = 0; str[i] != '\0'; i++) {
    hash += (hash << 5) + str[i];
  }
  return hash;
}

/**
 * Factory for instanciate each pointer vinculated to hmap header 
 * with memory setted to default nullnable value
 */
void *_instanciate_hmap_pointer(size_t ptr_size, char *ptr_name) {
  void *ptr = malloc(ptr_size);
  if (ptr == NULL) {
    printf("Error on %s allocation!\n", ptr_name);
    exit(1);
  }
  memset(ptr, 0, ptr_size);
  return ptr;
}

void hmap_print_header(HashMapHeader **header_ptr) {
  HashMapHeader *header = (HashMapHeader *)(*(header_ptr));

  printf("\n----------(Header)----------\n");
  printf("Reserved Space: %zu\n", header->reserved_size);
  printf("Allocated Data: %zu\n", header->allocated);

  printf("Key Idx Hmap: [");
  for (size_t i=0; i < header->reserved_size; i++) {
    printf("%zu, ", header->keys_idx_hmap[i]);
  }
  printf("]\n");
 
  printf("Key Idx: [");
  for (size_t i=0; i < header->reserved_size; i++) {
    printf("%zu, ", header->keys_idx[i]);
  }
  printf("]\n");

  printf("----------------------------\n");
}


