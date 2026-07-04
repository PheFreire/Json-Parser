#include "hashmap/hashmap.h"
#include "array/array.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long hash_to_djb2(unsigned char *str) {
  unsigned long hash = 5381;
  for (int i = 0; str[i] != '\0'; i++) {
    hash += (hash << 5) + str[i];
  }
  return hash;
}

void new_hmap(int **map_ptr, size_t size) {
  size_t map_size = (sizeof(**(map_ptr)) * (size)) + sizeof(HashMapHeader);
  HashMapHeader *header = (HashMapHeader *)realloc(*(map_ptr), map_size);

  if (header == NULL) {
    printf("Error on \"map_with_header\" allocation!\n");
    exit(1);
  }
  
  char **keys = NULL; 
  header[0] = (HashMapHeader){ .reserved_size=(size), .allocated=0, .keys=keys };

  (*(map_ptr)) = (void *)(header + 1);
  memset(*(map_ptr), 0, (size) * sizeof(**(map_ptr)));

  print_h_map_header(&header);
}

void print_h_map_header(HashMapHeader **header_ptr) {
  HashMapHeader header = (HashMapHeader)(**(header_ptr));

  printf("\n----------(Header)----------\n");
  printf("Reserved Space: %zu\n", header.reserved_size);
  printf("Allocated Data: %zu\n", header.allocated);
  printf("Keys: [");
    
  size_t keys_size = maplen(&header.keys);

  for (size_t i=0; i < keys_size; i++) {
    printf("\n  %s, ", (**(header_ptr)).keys[i]);
  }
  
  if (keys_size > 0) { printf("\n"); }
  printf("]\n");
  printf("----------------------------\n");
}



// Implementar:
// - hmap_append
//
