#include "hashmap/hashmap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Test hmap insert shoud add key in the right place, key_idx on the right place and hmap value on the right place and header should update correctly
// Test hmap insert should update correctly without incrementing the allocation size 
// get keys should iterate correctly
// get values should iterate correctly
// get items should iterate correctly

void test_hashmap_and_header_should_initialize_correctly() {
  int *h_map = NULL;

  new_hmap(&h_map, 32);

  HashMapHeader *header = get_hmap_header(&h_map);

  assert(header->reserved_size == 32);
  assert(header->allocated == 0);

  char **keys = NULL;
  get_hmap_keys(&h_map, keys);
  assert(keys == NULL);

  for (size_t i=0; i != 32; i++) {
    assert(header->keys[i] == NULL);
    assert(header->keys_idx[i] == 0);
    assert(header->keys_idx_hmap[i] == 0);
  }

  freehmap(&h_map);
}

void test_hash_function_should_generate_same_value_to_same_input() {
  unsigned char *value = (unsigned char *)"exemple 1";
  size_t hash_value_1 = (size_t)hash_to_djb2(value);
  size_t hash_value_2 = (size_t)hash_to_djb2(value);

  unsigned char *value_2 = (unsigned char *)"pato";
  size_t different_hash_value_1 = (size_t)hash_to_djb2(value_2);
  size_t different_hash_value_2 = (size_t)hash_to_djb2(value_2);

  assert(hash_value_1 == hash_value_2);
  assert(different_hash_value_1 == different_hash_value_2);
  assert(hash_value_1 != different_hash_value_1);
}

// Test hmap insert shoud add key in the right place, 
// key_idx on the right place and hmap value on the right place and header should update correctly
void test_hashmap_insert_should_position_hash_into_the_correct_place_on_key_idx() {
  int *h_map = NULL;
  new_hmap(&h_map, 100);

  char* key = "duck-test";
  size_t hash = (size_t)hash_to_djb2((unsigned char *)key);

  hmap_insert(&h_map, key, 10);
  freehmap(&h_map);
}
