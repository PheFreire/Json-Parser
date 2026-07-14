#include "hashmap/hashmap.h"
#include "stdio.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int *map = NULL;
  
  new_hmap(&map, 32);
  hmap_insert(&map, "pato1", 2);
  hmap_insert(&map, "pato2", 3);
  hmap_insert(&map, "pato3", 1);
  print_hmap(&map);

  hmapdel(&map, "pato2");
  print_hmap(&map);
}
