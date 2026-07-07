#include "array/array.h"
#include "hashmap/hashmap.h"
#include "stdio.h"
#include <stddef.h>
#include <stdlib.h>

int main() {
  int *map = NULL;
  
  new_hmap(&map, 300);
  hmap_add(&map, "pato", 10);
  hmap_add(&map, "abc", 11);
}
