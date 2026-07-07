#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct {
  size_t reserved_size;
  size_t allocated;
  char **keys;
  size_t *arrival_order_idx;
} HashMapHeader;

unsigned long hash_to_djb2(unsigned char *str);
void new_hmap(int **map_ptr, size_t size);
void print_hmap_header(HashMapHeader **header_ptr);
void hmap_add(int **map_ptr, char *key, int value);

#endif
