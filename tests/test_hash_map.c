#include "hashmap/hashmap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

void test_hashmap_and_header_should_initialize_correctly() {
  int *h_map = NULL;

  new_hmap(&h_map, 10);
}
