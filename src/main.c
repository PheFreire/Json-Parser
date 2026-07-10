#include "array/array.h"
#include "hashmap/hashmap.h"
#include "stdio.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int *map = NULL;
  
  new_hmap(&map);

  print_hmap(&map);
}
