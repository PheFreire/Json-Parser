#include "hashmap/hashmap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// Test hash function shoud generate same value to same input
// Test new hmap shoud iniciate pointers with correct start size
// Test hmap insert shoud add key in the right place, key_idx on the right place and hmap value on the right place and header should update correctly
// Test hmap insert should update correctly without incrementing the allocation size 
// get keys should iterate correctly
// get values should iterate correctly
// get items should iterate correctly

void test_hashmap_and_header_should_initialize_correctly() {
  int *h_map = NULL;

  new_hmap(&h_map, 32);
}
