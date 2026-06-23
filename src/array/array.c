#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Prints out the raw structural metrics tracked inside a Header variable.
 * 
 * Outputs a clean terminal layout representing current maximum capacity limits (Reserved Space) 
 * against real meaningful slots used (Allocated Data).
 *
 * @param h Copy of the specific runtime Header structure to inspect.
 */
void print_header(Header h) {
  printf("\n----------(Header)----------\n");
  printf("Reserved Space: %zu\n", h.reserved_size);
  printf("Allocated Data: %zu\n", h.allocated);
  printf("----------------------------\n");
}

/**
 * @brief Renders a full terminal visualization detailing both header metrics and underlying array contents.
 * 
 * Fetches the implicit shadow header object from the source array address space and triggers 
 * print_header(). Sequentially parses structural indexes, wrapping outputs into an itemized `[x, y, z]` layout.
 *
 * @param map The public array data reference pointer to trace visually.
 */
void print_map(int *map) {
  Header *loaded_header = get_header(map);
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

/**
 * @brief Retrieves the current logical utilization count of the dynamic array.
 * 
 * Fetches the implicit shadow header object from the source array address space 
 * to read and return the exact number of active elements currently populated 
 * within the user-facing data segment.
 *
 * @param map Pointer to the user's array pointer structure to evaluate.
 * @return size_t The total number of tracking indices currently utilized.
 */
size_t maplen(int **map) {
  Header *loaded_header = get_header(* map);
  return loaded_header->allocated;
}

