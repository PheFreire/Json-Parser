#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
  size_t reserved_size;
  size_t allocated;
} Header;

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
    (*(map)), sizeof(Header) + (sizeof(**(map)) * (size))            \
  );                                                                 \
  if (map_with_header == NULL) {                                     \
    printf("Error on \"newmap\" allocation!\n");                     \
    exit(1);                                                         \
  }                                                                  \
  map_with_header[0] = (Header){ .reserved_size=size, .allocated=0}; \
  (*(map)) = (void *)(map_with_header + 1);                          \
  memset(*(map), 0, size * sizeof(**(map)));                         \
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
 * overwrite the deletion target and decrement the size counter.
 *
 * @param map Pointer to the user's array pointer undergoing element deletion.
 * @param idx Zero-indexed numeric identifier of the item to discard.
 */
#define del(map, idx) do {                                                            \
  Header *header = get_header(*(map));                                                \
  if ((idx) >= header->allocated) {                                                   \
    printf("Invalid Index Error: Could Not Delete Element");                          \
    exit(1);                                                                          \
  }                                                                                   \
  uint8_t *map_ptr = (uint8_t *)(*(map));                                             \
  size_t data_size = sizeof(**(map));                                                 \
  uint8_t *dest = (map_ptr + ((idx) * data_size));                                    \
  uint8_t *src = (map_ptr + (((idx) + 1) * data_size));                               \
  size_t size_elements_to_move = ((header->allocated - (idx) - 1) * data_size);       \
  memmove(dest, src, size_elements_to_move);                                          \
  memset((map_ptr + ((header->allocated - 1) * data_size)), 0, data_size);            \
  header->allocated--;                                                                \
} while(0)

/**
 * @brief Removes an pointer from the array at the designated index.
 * 
 * Validates the targeted index constraints against logical boundary allocations. If valid, 
 * overwrite the deletion target, free the pointer and decrement the size counter.
 *
 * @param map Pointer to the user's array pointer undergoing element deletion.
 * @param idx Zero-indexed numeric identifier of the item to discard.
 */
#define del_ptr(map, idx) do {                                                        \
  free((*(map))[idx]);                                                                \
  del(map, idx);                                                                      \
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

/**
 * @brief Executes a sequential linear scan through the array to find a specific value.
 * 
 * Iterates through array indices up to the current logically populated boundary, evaluating 
 * items for an exact computational equality match against the target value.
 *
 * @param map Pointer to the user's array pointer structure to scan.
 * @param value The query value sought after.
 * @return Returns the tracking index of the initial match instance, or -1 if missing.
 */
#define index_of(map, value) ({                     \
  Header *header = get_header(*(map));              \
  int found_idx = -1;                               \
  for (size_t i = 0; i != header->allocated; i++) { \
    if ((*(map))[i] == (value)) {                   \
      found_idx = (int)i;                           \
      break;                                        \
    }                                               \
  }                                                 \
  found_idx;                                        \
})

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
#define maplen(map) (get_header(* (map))->allocated)

/**
 * @brief Prints out the raw structural metrics tracked inside a Header variable.
 * 
 * Outputs a clean terminal layout representing current maximum capacity limits (Reserved Space) 
 * against real meaningful slots used (Allocated Data).
 *
 * @param h Copy of the specific runtime Header structure to inspect.
 */
#define print_header(h) do {                        \
  printf("\n----------(Header)----------\n");       \
  printf("Reserved Space: %zu\n", h.reserved_size); \
  printf("Allocated Data: %zu\n", h.allocated);     \
  printf("----------------------------\n");         \
} while(0)

/**
 * @brief Renders a full terminal visualization detailing both header metrics and underlying array contents.
 * 
 * Fetches the implicit shadow header object from the source array address space and triggers 
 * print_header(). Sequentially parses structural indexes, wrapping outputs into an itemized `[x, y, z]` layout.
 *
 * @param map The public array data reference pointer to trace visually.
 */

#define __print_map_wrapper(map, fmt, type_cast) do {                 \
  Header *loaded_header = get_header(map);                            \
  Header header = loaded_header[0];                                   \
  print_header(header);                                               \
  printf("\n----------(Map)----------\n");                            \
  printf("[");                                                        \
  for (size_t i = 0; i < header.reserved_size; i++) {                 \
    printf(fmt, ((type_cast *)(map))[i]);                             \
    if (i != (header.reserved_size - 1)) printf(", ");                \
  }                                                                   \
  printf("]\n");                                                      \
  printf("-------------------------\n");                              \
  printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");   \
} while (0)

static inline void __print_map_char(char **map) {
  __print_map_wrapper((* (map)), "%c", char);
}

static inline void __print_map_int(int **map) {
  __print_map_wrapper((* (map)), "%d", int);
}

static inline void __print_map_string(char ***map) {
  __print_map_wrapper((* (map)), "%s", char *);
}

static inline void __print_map_size_t(size_t **map) {
  __print_map_wrapper((* (map)), "%zu", size_t);
}

// Should receive the main pointer and not a pointer to the main pointer
#define print_map(map)                  \
  _Generic((map),                       \
    char**:   __print_map_char,          \
    int**:    __print_map_int,           \
    char***:  __print_map_string,        \
    size_t**: __print_map_size_t,        \
    default: __print_map_int            \
  )(map)

#endif
