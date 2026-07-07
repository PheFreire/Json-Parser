#include "hashmap/hashmap.h"
#include "array/array.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

void new_hmap(int **map_ptr, size_t size) {
  if (map_ptr == NULL || (*(map_ptr)) != NULL) {
    printf("Error HashMap already initialized!\n");
    exit(1);
  }

  size_t map_size = (sizeof(**map_ptr) * size) + sizeof(HashMapHeader);
  void *map = malloc(map_size);
  if (map == NULL) {
    printf("Error on HashMap allocation!\n");
    exit(1);
  }
  memset(map, 0, map_size);

  HashMapHeader *header = (HashMapHeader *)map;
  
  char **keys = NULL;
  newmap(&keys, size);

  size_t *arrival_order_idx = NULL;
  newmap(&arrival_order_idx, size);

  header[0] = (HashMapHeader){ .reserved_size=(size), .allocated=0, .keys=keys, .arrival_order_idx=arrival_order_idx};
  *map_ptr = (void *)(header + 1);
}

void print_hmap_header(HashMapHeader **header_ptr) {
  HashMapHeader *header = (HashMapHeader *)(*(header_ptr));

  printf("\n----------(Header)----------\n");
  printf("Reserved Space: %zu\n", header->reserved_size);
  printf("Allocated Data: %zu\n", header->allocated);
  printf("----------------------------\n");
}

void print_hmap(int **hmap_ptr) {
  HashMapHeader *header = get_hmap_header(hmap_ptr);

  printf("\n----------------------------\n\n");
  printf("{");
  for (size_t i=0; i < header->allocated; i++) {
    size_t idx = header->arrival_order_idx[i]; 
    printf("\n  %s: %d, ", header->keys[idx], (*(hmap_ptr))[idx]);
  }
  if (header->allocated > 0) { printf("\n"); }
  printf("}\n");
}

void hmap_add(int **map_ptr, char *key, int value) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  Header *keys_header = get_header(hmap_header->keys);

  size_t hash_key = hash_to_djb2((unsigned char *)(key)) % keys_header->reserved_size;

  hmap_header->keys[hash_key] = key;
  keys_header->allocated++;

  (* (map_ptr))[hash_key] = value;
  hmap_header->allocated++;
  
  append(&hmap_header->arrival_order_idx, hash_key);

  print_hmap(map_ptr);
}


// O get_header da array deveria pedir ponteiro do ponteiro
// O print_header da array deveria pedir ponteiro do ponteiro
// Tirar as informacoes do Header do print_map da array
// Implementar o _dinamic_expand_hmap
  // Criar novo keys vazio com tamanho novo 
  // Criar novo hmap vazio com tamanho novo
  // Criar novo arrival_order_idx com tamanho novo

  // Iterar cada elemento de "idx for idx in arrival_order_idx"
    // Iterar o ponteiro de keys antigo com cada uma das idx de arrival_order_idx "keys[idx]"
    // Gerar o novo valor de hash com a key "new_hash = hash_to_djb2(keys[idx])"
    // Alocar a key no novo keys com a posicao de index da key sendo o novo valor de hash "new_keys[new_hash] = keys[idx]"
    // Alocar o value do hmap no novo hmap com a posicao de index do value sendo o novo valor de hash "new_hmap[new_hash] = hmap[idx]"
    // Atualizar os valores de arrival_order_idx para os novos valores de hash "arrival_order_idx[idx] = hash_to_djb2(keys[idx])"
  
  // Dar free nos ponteiros antigos
  // Atualizar os headers
