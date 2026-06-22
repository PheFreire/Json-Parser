#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes or resizes a dynamic array by allocating space for the shadow header.
 * 
 * This function calculates the total memory block size needed for the Header struct 
 * combined with the requested number of 'int' elements. Upon successful allocation, 
 * the user's external pointer is shifted forward to point directly at the usable data segment.
 *
 * @param map Pointer to the user's array pointer to be initialized or reallocated.
 * @param size Initial count of integer elements to reserve in memory.
 */
void newmap(int **map, size_t size) {
  Header *map_with_header = (Header *)realloc((* map), sizeof(Header) + (sizeof(** map) * size));

  if (map_with_header == NULL) {
    printf("Error on \"map_with_header\" allocation!\n");
    exit(1);
  }

  map_with_header[0] = (Header){ .reserved_size=size, .allocated=0};
  (* map) = (int *)(map_with_header + 1);
}

/**
 * @brief Retrieves the memory address of the hidden shadow header associated with the array.
 * 
 * Uses reverse pointer arithmetic to subtract the size of a Header structure 
 * from the user-facing data pointer, exposing the hidden operational metadata.
 *
 * @param map The public data pointer managed by the framework.
 * @return Header* Pointer to the hidden structure tracking capacity and utilization.
 */
Header *get_header(int *map) {
  Header *raw_ptr = (Header *)map;
  Header *header_ptr = raw_ptr - 1;

  return header_ptr;
}

void freemap(int **map) {
  if (map == NULL || *map == NULL) return;
  Header *header_ptr = get_header(* map);
  free(header_ptr);
  (* map) = NULL;
}

/**
 * @brief Expands the array capacity using a dampening geometric growth formula.
 * 
 * Internal routine that calculates a new capacity threshold utilizing bitwise shifts 
 * and constants to minimize the amortized cost of memory reallocations. Reallocates 
 * the monolithic block (Header + Data) and updates both structural references.
 *
 * @param map Pointer to the user's array pointer to receive the new data segment address.
 * @param header_ptr Pointer to the header pointer that will be updated with the new capacity.
 */
void _dinamic_expand(int **map, Header **header_ptr) {
  size_t new_total_size = (
    (* header_ptr)->reserved_size +
    ((* header_ptr)->reserved_size >> 3) + 
    ((* header_ptr)->reserved_size < 9 ? 3 : 6)
  );

  Header *new_map = (Header *)realloc(
    (* header_ptr), 
    sizeof(Header) + (sizeof(** map) * new_total_size)
  );

  if (new_map == NULL) {
    printf("Error on dinamic reallocate map\n");
    exit(1);
  }
  (* header_ptr) = new_map;
  (* header_ptr)->reserved_size = new_total_size;
  (* map) = (int *)((* header_ptr) + 1);
}

/**
 * @brief Appends a new integer value to the tail end of the dynamic array.
 * 
 * Checks if logical usage has hit the physical capacity threshold. If so, triggers 
 * the dynamic capacity expansion routine before binding the data value into the 
 * first open slot and incrementing the logical counter.
 *
 * @param map Pointer to the user's array pointer where the data should be appended.
 * @param data The raw integer value to store.
 */
void append(int **map, int data) {
  Header *header = get_header(* map);
  
  if (header->reserved_size == header->allocated) {
    _dinamic_expand(map, &header);
  }
  
  (* map)[header->allocated] = data;
  header->allocated++;
}

/**
 * @brief Inserts an integer value at an arbitrary index, shifting subsequent elements right.
 * 
 * If the target index equals or exceeds current logical allocation boundaries, it defaults 
 * to an append() execution path. Otherwise, guarantees memory padding, shifts downstream data 
 * blocks using memmove, and overrides the targeted block index.
 *
 * @param map Pointer to the user's array pointer hosting the insertion.
 * @param idx Zero-indexed target position where the data should be injected.
 * @param value The raw integer value to write.
 */
void insert(int **map, size_t idx, int value) {
  Header *header = get_header(*map);

  if (idx >= header->allocated) {
    append(map, value);
    return;
  }
  
  if (header->reserved_size == header->allocated) {
    _dinamic_expand(map, &header);
  }
  
  size_t size_target_til_end = (header->allocated - idx) * sizeof(int);
  int *ptr_to_target = (*map) + idx;
  

  memmove((*map) + idx + 1, ptr_to_target, size_target_til_end);
  (* map)[idx] = value;
  header->allocated++;
}

/**
 * @brief Removes an item from the array at the designated index.
 * 
 * Validates the targeted index constraints against logical boundary allocations. If valid, 
 * leverages a memmove call block to shift subsequent items leftwards by one unit, 
 * overwriting the deletion target and decrementing the size counter.
 *
 * @param map Pointer to the user's array pointer undergoing element deletion.
 * @param idx Zero-indexed numeric identifier of the item to discard.
 */
void del(int **map, size_t idx) {
  Header *header = get_header(*map);

  if (idx >= header->allocated) {
    printf("Invalid Index Error: Could Not Delete Element");
    exit(1);
  }
  
  size_t size_target_til_end = (header->allocated - idx) * sizeof(int);
  int *ptr_after_target = (*map) + idx + 1;

  memmove((*map) + idx, ptr_after_target, size_target_til_end);
  header->allocated--;
}

/**
 * @brief Executes a sequential linear scan through the array to find a specific value.
 * 
 * Iterates through array indices up to the current logically populated boundary, evaluating 
 * items for an exact computational equality match against the target value.
 *
 * @param map Pointer to the user's array pointer structure to scan.
 * @param value The query integer value sought after.
 * @return int Returns the tracking index of the initial match instance, or -1 if missing.
 */
int index_of(int **map, int value) {
  Header *header = get_header(* map);
  for (size_t i = 0; i != header->allocated; i++) {
    if ((* map)[i] == value) return i;
  }
  return -1;
}

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

// Fazer o replace by idx
// Fazer o replace by value
