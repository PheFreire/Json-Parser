#include "utils/delete_from_ptr.h"
#include "hashmap/hashmap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

HashMapHeader *get_hmap_header(int **map_ptr) {
  return (((HashMapHeader *)(*(map_ptr))) - 1);
}

size_t hash_to_djb2(unsigned char *str) {
  unsigned long hash = 5381;
  for (int i = 0; str[i] != '\0'; i++) {
    hash += (hash << 5) + str[i];
  }
  return hash;
}

void *__instanciate_hmap_pointer(size_t ptr_size, char *ptr_name) {
  void *ptr = malloc(ptr_size);
  if (ptr == NULL) {
    printf("Error on %s allocation!\n", ptr_name);
    exit(1);
  }
  memset(ptr, 0, ptr_size);
  return ptr;
}

void new_hmap(int **map_ptr, size_t count) {
  if (count < BASE_HMAP_SIZE) {
    count = BASE_HMAP_SIZE;
  }

  if (map_ptr == NULL || (*(map_ptr)) != NULL) {
    printf("Error HashMap already initialized!\n");
    exit(1);
  }

  void *hmap = __instanciate_hmap_pointer(
    (sizeof(**(map_ptr)) * (count)) + sizeof(HashMapHeader), "HashMap"
  );
  size_t *keys_idx_hmap = __instanciate_hmap_pointer(
    (sizeof(size_t) * (count)), "HashMap Keys Idx Hmap"
  );
  size_t *keys_idx = __instanciate_hmap_pointer(
    (sizeof(size_t) * (count)), "HashMap Keys Idx"
  );
  char **keys = __instanciate_hmap_pointer(
    (sizeof(char *) * (count)), "HashMap Keys"
  );

  HashMapHeader *header = (HashMapHeader *)hmap;
  header[0] = (HashMapHeader){ 
    .reserved_size=(count),
    .allocated=0, 
    .keys=keys, 
    .keys_idx=keys_idx,
    .keys_idx_hmap=keys_idx_hmap,
  };

  (*(map_ptr)) = (void *)(header + 1);
}

void print_hmap_header(HashMapHeader **header_ptr) {
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

void print_hmap(int **hmap_ptr) {
  HashMapHeader *header = get_hmap_header(hmap_ptr);
  int *hmap = (*(hmap_ptr));

  print_hmap_header(&header);
  printf("{");
  for (size_t i=0; i < header->allocated; i++) {
    size_t idx = header->keys_idx[i];
    printf("\n  %s: %d, ", header->keys[idx], hmap[idx]);
  }
  if (header->allocated > 0) { printf("\n"); }
  printf("}\n");
}

void __free_hmap_header(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  
  free(hmap_header->keys_idx_hmap);
  free(hmap_header->keys_idx);
  free(hmap_header->keys);
  free(hmap_header);
}

void _dinamic_expand_hmap(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);

  size_t new_count = (
    hmap_header->reserved_size +
    (hmap_header->reserved_size / 3) +
    (hmap_header->reserved_size >> 3) +
    (hmap_header->reserved_size < 9 ? 3 : 6)
  );
  
  size_t ptr_element_size = sizeof(**(map_ptr));

  void *new_hmap = __instanciate_hmap_pointer(
    (ptr_element_size * new_count) + sizeof(HashMapHeader), "HashMap"
  );
  size_t *new_keys_idx_hmap = __instanciate_hmap_pointer(
    (sizeof(size_t) * new_count), "HashMap Keys Idx"
  );
  size_t *new_keys_idx = __instanciate_hmap_pointer(
    (sizeof(size_t) * new_count), "HashMap Keys Idx"
  );
  char **new_keys = __instanciate_hmap_pointer(
    (sizeof(char *) * new_count), "HashMap Keys"
  );

  HashMapHeader *new_header = (HashMapHeader *)new_hmap;
  new_header[0] = (HashMapHeader){ 
    .reserved_size=new_count,
    .allocated=hmap_header->allocated,
    .keys=new_keys,
    .keys_idx=new_keys_idx,
    .keys_idx_hmap=new_keys_idx_hmap,
  };

  new_hmap = (void *)(new_header + 1);
  
  for (size_t i=0; i < hmap_header->allocated; i++) {
    size_t key_idx_by_arrive_order = hmap_header->keys_idx[i];

    char *key = hmap_header->keys[key_idx_by_arrive_order];
    size_t new_hash = (hash_to_djb2((unsigned char *)(key)) % new_count);

    new_keys[new_hash] = key;
    new_keys_idx_hmap[new_hash] = i;
    memcpy(
      (char *)new_hmap + (new_hash * ptr_element_size),
      (char *)(* map_ptr) + (key_idx_by_arrive_order * ptr_element_size),
      ptr_element_size
    );

    new_keys_idx[i] = new_hash;
    printf("%zu\n", new_hash);
  }
  
  __free_hmap_header(map_ptr);
  (*(map_ptr)) = new_hmap;
}

void hmap_insert(int **map_ptr, char *key, int value) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);

  size_t hash_key = (hash_to_djb2((unsigned char *)(key)) % hmap_header->reserved_size);
  bool already_exists_on_hmap = ((hmap_header->keys[hash_key]) != NULL);

  if (
    hmap_header->allocated == hmap_header->reserved_size && 
    !already_exists_on_hmap
  ) {
    _dinamic_expand_hmap(map_ptr);
    hmap_header = get_hmap_header(map_ptr);
  }
  
  hmap_header->keys_idx[hmap_header->allocated] = hash_key;
  hmap_header->keys_idx_hmap[hash_key] = hmap_header->allocated;

  (*(map_ptr))[hash_key] = value;
  hmap_header->keys[hash_key] = key;

  if (!already_exists_on_hmap) {
    hmap_header->allocated++;
  }
}

char **get_hmap_keys(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  char **ordered_keys = calloc(hmap_header->allocated + 1, sizeof(char *));

  for (size_t i=0; i < hmap_header->allocated; i++) {
    size_t idx = hmap_header->keys_idx[i];
    ordered_keys[i] = hmap_header->keys[idx];
  } 
  
  ordered_keys[hmap_header->allocated + 1] = NULL;
  return ordered_keys;
}

void *get_hmap_values(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);

  size_t value_size = sizeof((**(map_ptr)));
  void *ordered_values = calloc(hmap_header->allocated, value_size);

  for (size_t i=0; i < hmap_header->allocated; i++) {
    size_t idx = hmap_header->keys_idx[i];
    memcpy(
      ordered_values + (value_size * i), 
      (* (map_ptr)) + (value_size * idx), 
      value_size
    );
  }

  return ordered_values;
}

size_t hmaplen(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  return hmap_header->allocated;
}

void hmapdel(int **map_ptr, char *key) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  size_t hash = (hash_to_djb2((unsigned char *)(key)) % hmap_header->reserved_size);

  size_t keys_idx_pos = hmap_header->keys_idx_hmap[hash];

  del_element(((void **)map_ptr), hash, sizeof(**(map_ptr)));

  del_element((void **)&hmap_header->keys, hash, sizeof(char *));
  del_element((void **)&hmap_header->keys_idx, keys_idx_pos, sizeof(size_t));
  del_element_and_resize((void **)&hmap_header->keys_idx, keys_idx_pos, sizeof(size_t), hmap_header->allocated);

  hmap_header->allocated--;
}
