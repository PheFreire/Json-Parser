#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct {
  size_t reserved_size;
  size_t allocated;
  char **keys;
} HashMapHeader;

unsigned long hash_to_djb2(unsigned char *str);
void new_hmap(int **map_ptr, size_t size);
void print_h_map_header(HashMapHeader **header_ptr);

#endif
