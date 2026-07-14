#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#define BASE_HMAP_SIZE 32

typedef struct {
  size_t reserved_size;
  size_t allocated;
  char **keys;
  size_t *keys_idx;
  size_t *keys_idx_hmap;

} HashMapHeader;

unsigned long hash_to_djb2(unsigned char *str);
void new_hmap(int **map_ptr, size_t count);
void print_hmap_header(HashMapHeader **header_ptr);
void print_hmap(int **hmap_ptr);
void hmap_insert(int **map_ptr, char *key, int value);

char **get_hmap_keys(int **map_ptr);
void *get_hmap_values(int **map_ptr);
size_t hmaplen(int **map_ptr);
void hmapdel(int **map_ptr, char *key);

#endif
