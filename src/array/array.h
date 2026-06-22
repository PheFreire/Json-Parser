#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t reserved_size;
  size_t allocated;
} Header;

int index_of(int **map, int value);

void print_header(Header h);
void print_map(int *map);

size_t maplen(int **map);

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
#define _dinamic_expand(map, header_ptr) do {            \
  size_t new_total_size = (                              \
    (* header_ptr)->reserved_size +                      \
    ((* header_ptr)->reserved_size >> 3) +               \
    ((* header_ptr)->reserved_size < 9 ? 3 : 6)          \
  );                                                     \
  Header *new_map = (Header *)realloc(                   \
    (* header_ptr),                                      \
    sizeof(Header) + (sizeof(**(map)) * new_total_size)  \
  );                                                     \
  if (new_map == NULL) {                                 \
    printf("Error on dinamic reallocate map\n");         \
    exit(1);                                             \
  }                                                      \
  (*(header_ptr)) = new_map;                             \
  ((*(header_ptr))->reserved_size) = new_total_size;     \
  (*(map)) = (void *)((*(header_ptr)) + 1);              \
} while(0)

/**
 * @brief Initializes or resizes a dynamic array by allocating space for the shadow header.
 * 
 * This function calculates the total memory block size needed for the Header struct 
 * combined with the requested number of elements. Upon successful allocation, 
 * the user's external pointer is shifted forward to point directly at the usable data segment.
 *
 * @param map Pointer to the user's array pointer to be initialized or reallocated.
 * @param size Initial count of elements to reserve in memory.
 */
#define newmap(map, size) do {                                       \
  Header *map_with_header = (Header *)realloc(                       \
    (*(map)), sizeof(Header) + (sizeof(**(map)) * (size))                \
  );                                                                 \
  if (map_with_header == NULL) {                                     \
    printf("Error on \"map_with_header\" allocation!\n");            \
    exit(1);                                                         \
  }                                                                  \
  map_with_header[0] = (Header){ .reserved_size=size, .allocated=0}; \
  (*(map)) = (void *)(map_with_header + 1);                            \
} while(0)

/**
 * @brief Retrieves the memory address of the hidden shadow header associated with the array.
 * 
 * Uses reverse pointer arithmetic to subtract the size of a Header structure 
 * from the user-facing data pointer, exposing the hidden operational metadata.
 *
 * @param map The public data pointer managed by the framework.
 * @return Header* Pointer to the hidden structure tracking capacity and utilization.
 */
#define get_header(map) (((Header *)(map)) - 1)

/**
 * @brief Appends a new value to the tail end of the dynamic array.
 * 
 * Checks if logical usage has hit the physical capacity threshold. If so, triggers 
 * the dynamic capacity expansion routine before binding the data value into the 
 * first open slot and incrementing the logical counter.
 *
 * @param map Pointer to the user's array pointer where the data should be appended.
 * @param data The raw value to store.
 */
#define append(map, data) do {                      \
  Header *header = get_header(*(map));              \
  if (header->reserved_size == header->allocated) { \
    _dinamic_expand(map, &header);                  \
  }                                                 \
  (* map)[header->allocated] = data;                \
  header->allocated++;                              \
} while(0)

/**
 * @brief Inserts an value at an arbitrary index, shifting subsequent elements right.
 * 
 * If the target index equals or exceeds current logical allocation boundaries, it defaults 
 * to an append() execution path. Otherwise, guarantees memory padding, shifts downstream data 
 * blocks using memmove, and overrides the targeted block index.
 *
 * @param map Pointer to the user's array pointer hosting the insertion.
 * @param idx Zero-indexed target position where the data should be injected.
 * @param value The raw value to write.
 */
#define insert(map, idx, value) do {                                           \
  Header *header = get_header(*(map));                                         \
  if (idx >= header->allocated) {                                              \
    append(map, value);                                                        \
  }                                                                            \
  else {                                                                       \
    if (header->reserved_size == header->allocated) {                          \
      _dinamic_expand(map, &header);                                           \
    }                                                                          \
    size_t size_target_til_end = (header->allocated - idx) * sizeof(**(map));  \
    int *ptr_to_target = (*(map)) + idx;                                       \
    memmove((*(map)) + idx + 1, ptr_to_target, size_target_til_end);           \
    (*(map))[idx] = value;                                                     \
    header->allocated++;                                                       \
  }                                                                            \
} while(0)

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
#define del(map, idx) do {                                                    \
  Header *header = get_header(*(map));                                        \
  if ((idx) >= header->allocated) {                                           \
    printf("Invalid Index Error: Could Not Delete Element");                  \
    exit(1);                                                                  \
  }                                                                           \
  size_t size_target_til_end = (header->allocated - (idx)) * sizeof(**(map)); \
  int *ptr_after_target = (*(map)) + (idx) + 1;                               \
  memmove((*(map)) + (idx), ptr_after_target, size_target_til_end);           \
  header->allocated--;                                                        \
} while(0)

/**
 * @brief Deallocates the dynamic array memory, including its hidden shadow header.
 * 
 * Safely evaluates both the external pointer and the internal data segment address. 
 * Locates the hidden Header via reverse pointer arithmetic, releases the monolithic 
 * memory block from the heap, and forces the user's reference to NULL to prevent 
 * dangling pointer vulnerabilities.
 *
 * @param map Pointer to the user's array pointer targeted for destruction.
 */
#define freemap(map) {                          \
  if ((map) == NULL || *(map) == NULL) return;  \
  Header *header_ptr = get_header(* (map));     \
  free(header_ptr);                             \
  (*(map)) = NULL;                              \
} while(0)

#endif
