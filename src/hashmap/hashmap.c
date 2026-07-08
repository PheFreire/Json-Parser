#include "hashmap/hashmap.h"
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
  if (map_ptr == NULL || (*(map_ptr)) != NULL) {
    printf("Error HashMap already initialized!\n");
    exit(1);
  }

  void *hmap = __instanciate_hmap_pointer(
    (sizeof(**(map_ptr)) * (count)) + sizeof(HashMapHeader), "HashMap"
  );
  size_t *keys_idx = __instanciate_hmap_pointer(
    (sizeof(size_t) * count), "HashMap Keys Idx"
  );
  char **keys = __instanciate_hmap_pointer(
    (sizeof(char *) * count), "HashMap Keys"
  );

  HashMapHeader *header = (HashMapHeader *)hmap;
  header[0] = (HashMapHeader){ 
    .reserved_size=(count), 
    .allocated=0, 
    .keys=keys, 
    .keys_idx=keys_idx,
  };

  (*(map_ptr)) = (void *)(header + 1);
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
    size_t idx = header->keys_idx[i]; 
    printf("\n  %s: %d, ", header->keys[idx], (*(hmap_ptr))[idx]);
  }
  if (header->allocated > 0) { printf("\n"); }
  printf("}\n");
}

void __free_hmap_header(int **map_ptr) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);
  
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
  };

  new_hmap = (void *)(new_header + 1);
  
  for (size_t i=0; i < hmap_header->reserved_size; i++) {
    size_t key_idx_by_arrive_order = hmap_header->keys_idx[i];

    char *key = hmap_header->keys[key_idx_by_arrive_order];
    size_t new_hash = hash_to_djb2((unsigned char *)(key)) % new_count;

    new_keys[new_hash] = key;
    memcpy(
      (char *)new_hmap + (new_hash * ptr_element_size),
      (char *)(*map_ptr) + (key_idx_by_arrive_order * ptr_element_size),
      ptr_element_size
    );

    new_keys_idx[i] = new_hash;
  }

  __free_hmap_header(map_ptr);
}

void hmap_add(int **map_ptr, char *key, int value) {
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);

  size_t hash_key = hash_to_djb2((unsigned char *)(key)) % hmap_header->reserved_size;

  (*(map_ptr))[hash_key] = value;
  hmap_header->keys[hash_key] = key;
  hmap_header->keys_idx[hmap_header->allocated] = hash_key;

  hmap_header->allocated++;
}


// O get_header da array deveria pedir ponteiro do ponteiro
// O print_header da array deveria pedir ponteiro do ponteiro
// Tirar as informacoes do Header do print_map da array
// Implementar verificacao do ponteiro nao ser NULL e possuir um header no inicio de todo fluxo de array e hashmap

// Implementar o _dinamic_expand_hmap (X)
  // Criar novo keys vazio com tamanho novo 
  // Criar novo hmap vazio com tamanho novo
  // Criar novo keys_idx com tamanho novo

  // Iterar cada elemento de "idx for idx in keys_idx"
    // Iterar o ponteiro de keys antigo com cada uma das idx de keys_idx "keys[idx]"
    // Gerar o novo valor de hash com a key "new_hash = hash_to_djb2(keys[idx])"
    // Alocar a key no novo keys com a posicao de index da key sendo o novo valor de hash "new_keys[new_hash] = keys[idx]"
    // Alocar o value do hmap no novo hmap com a posicao de index do value sendo o novo valor de hash "new_hmap[new_hash] = hmap[idx]"
    // Atualizar os valores de keys_idx para os novos valores de hash "keys_idx[idx] = hash_to_djb2(keys[idx])"
  
  // Dar free nos ponteiros antigos
  // Atualizar os headers
