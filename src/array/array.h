#include <stddef.h>

#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
  size_t reserved_size;
  size_t allocated;
} Header;

void newmap(int **map, size_t size);
void append(int **map, int data);
void insert(int **map, size_t idx, int value);
void del(int **map, size_t idx);

int index_of(int **map, int value);

void print_header(Header h);
void print_map(int *map);

Header *get_header(int *map);
size_t maplen(int **map);
void freemap(int **map);

#endif
