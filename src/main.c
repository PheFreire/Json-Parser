#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
  size_t reserved_size;
  size_t allocated;
} Header;

void print_header(Header h) {
  printf("\n----------(Header)----------\n");
  printf("Reserved Space: %zu\n", h.reserved_size);
  printf("Allocated Data: %zu\n", h.allocated);
  printf("----------------------------\n");
}

Header *_get_header(int *map) {
  Header *raw_ptr = (Header *)map;
  Header *header_ptr = raw_ptr - 1;

  return header_ptr;
}

void print_map(int *map) {
  Header *loaded_header = _get_header(map);
  Header header = loaded_header[0];

  print_header(header);

  printf("\n----------(Map)----------\n");
  printf("[");
  for (size_t i = 0; i != header.reserved_size; i++) {
    if (i != header.reserved_size-1) {
      printf("%d, ", map[i]);
    }

    else {
      printf("%d", map[i]);
    }
  }
  printf("]\n");
  printf("-------------------------\n");

  printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
}

void newmap(int **map, size_t size) {
  Header *map_with_header = (Header *)realloc((* map), sizeof(Header) + (sizeof(** map) * size));

  if (map_with_header == NULL) {
    printf("Error on \"map_with_header\" allocation!\n");
    exit(1);
  }

  map_with_header[0] = (Header){ .reserved_size=size, .allocated=0};
  (* map) = (int *)(map_with_header + 1);
}

void _dinamic_expand(int **map, Header **header_ptr) {
  size_t new_total_size = (
    (* header_ptr)->reserved_size +
    ((* header_ptr)->reserved_size >> 3) + 
    ((* header_ptr)->reserved_size < 9 ? 3 : 6)
  );

  Header *new_map = (Header *)realloc((* header_ptr), sizeof(Header) + (sizeof(** map) * new_total_size));

  if (new_map == NULL) {
    printf("Error on dinamic reallocate map\n");
    exit(1);
  }
  (* header_ptr) = new_map;
  (* header_ptr)->reserved_size = new_total_size;
  (* map) = (int *)((* header_ptr) + 1);
}

void append(int **map, int data) {
  Header *header = _get_header(* map);
  
  if (header->reserved_size == header->allocated) {
    _dinamic_expand(map, &header);
  }
  
  (* map)[header->allocated] = data;
  header->allocated++;
}

void del(int **map, size_t idx) {
  Header *header = _get_header(*map);

  if (idx >= header->allocated) {
    printf("Invalid Index Error: Could Not Delete Element");
    exit(1);
  }
  
  size_t size_target_til_end = (header->allocated - idx) * sizeof(int);
  int *ptr_after_target = (*map) + idx + 1;

  memmove((*map) + idx, ptr_after_target, size_target_til_end);
  header->allocated--;
}

void insert(int **map, size_t idx, int value) {
  Header *header = _get_header(*map);

  if (idx >= header->allocated) {
    append(map, value);
    return;
  }
  
  if (header->reserved_size == header->allocated) {
    _dinamic_expand(map, &header);
  }
  
  size_t size_target_til_end = (header->allocated - idx) * sizeof(int);
  int *ptr_to_target = (*map) + idx;
  

  memmove((*map) + idx + 1, ptr_to_target, size_target_til_end);
  (* map)[idx] = value;
  header->allocated++;
}

int index_of(int **map, int value) {
  Header *header = _get_header(* map);

  for (size_t i = 0; i != header->allocated; i++) {
    if ((* map)[i] == value) return i;
  }

  return -1;
}

int main() {
  printf("%d\n", 12345%10);
  int *map = NULL;

  newmap(&map, 2);

  append(&map, 12);
  append(&map, 13);
  append(&map, 14);
  append(&map, 15);
  append(&map, 16);
  append(&map, 17);
  
  print_map(map);
  insert(&map, 0, 1);
  print_map(map);

  printf("%d\n", index_of(&map, 0));
}
