#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define BASE_HMAP_SIZE 32

#include <string.h>
#include <stdbool.h>

/**
 * @brief Hidden shadow header tracking a dynamic hashmap's capacity, usage and key bookkeeping.
 *
 * @param reserved_size Total number of hash slots currently allocated for the hashmap.
 * @param allocated Number of key/value pairs currently populated in the hashmap.
 * @param keys Array of hashmap keys (char*).
 * Each key (char *) is stored at the index corresponding to its hash (size_t),
 * computed as size_t hash_to_djb2(key).
 *
 * @param keys_idx Array storing the hashes (size_t) of the keys (char *) in arrival order,
 * with the first hash (size_t) generated stored at index 0.
 *
 * @param keys_idx_hmap Array storing, at the index corresponding to a
 * key's hash, the insertion order (arrival number) of that key in the hashmap.
 */
typedef struct {
  size_t reserved_size;
  size_t allocated;
  char **keys;
  size_t *keys_idx;
  size_t *keys_idx_hmap;

} HashMapHeader;

/**
 * @brief Allocates and zero-initializes a memory block bound to a hashmap's internal bookkeeping arrays.
 *
 * Factory for instanciate each pointer vinculated to hmap header
 * with memory setted to default nullnable value.
 *
 * @param ptr_size Total number of bytes to allocate.
 * @param ptr_name Human readable label used in the allocation failure message.
 * @return void* Pointer to the newly allocated, zeroed memory block.
 */
void *_instanciate_hmap_pointer(size_t ptr_size, char *ptr_name);

/**
 * @brief Computes the djb2 hash digest of a null-terminated string.
 *
 * @param str The key string to hash.
 * @return unsigned long The resulting hash value.
 */
unsigned long hash_to_djb2(unsigned char *str);

/**
 * @brief Retrieves the memory address of the hidden shadow header associated with the hashmap.
 *
 * Uses reverse pointer arithmetic to subtract the size of a HashMapHeader structure
 * from the user-facing data pointer, exposing the hidden operational metadata.
 *
 * @param map_ptr The public data pointer managed by the framework.
 * @return HashMapHeader* Pointer to the hidden structure tracking capacity, usage and keys.
 */
#define get_hmap_header(map_ptr) (((HashMapHeader *)(*(map_ptr))) - 1)

/**
 * @brief Retrieves the current logical utilization count of the hashmap.
 *
 * Fetches the implicit shadow header object from the source hashmap address space
 * to read and return the exact number of key/value pairs currently populated.
 *
 * @param map_ptr Pointer to the user's hashmap pointer structure to evaluate.
 * @return size_t The total number of key/value pairs currently stored.
 */
#define hmaplen(map_ptr) ((HashMapHeader *)get_hmap_header((map_ptr))->allocated)

/**
 * @brief Initializes a hashmap by allocating space for the shadow header and its key bookkeeping arrays.
 *
 * Enforces a minimum reserved size of BASE_HMAP_SIZE, allocates the monolithic value block
 * (HashMapHeader + Data) plus the separate keys, keys_idx and keys_idx_hmap arrays, and binds
 * the user's external pointer to the usable data segment. Fails loudly if the map is already initialized.
 *
 * @param map_ptr Pointer to the user's hashmap pointer to be initialized.
 * @param count Initial number of hash slots to reserve; clamped up to BASE_HMAP_SIZE.
 */
#define new_hmap(map_ptr, count) do {                                         \
  size_t hmap_count = (size_t) count;                                         \
  if ((hmap_count) < BASE_HMAP_SIZE) {                                        \
    (hmap_count) = BASE_HMAP_SIZE;                                            \
  }                                                                           \
  if (map_ptr == NULL || (*(map_ptr)) != NULL) {                              \
    printf("Error HashMap already initialized!\n");                           \
    exit(1);                                                                  \
  }                                                                           \
  void *hmap = _instanciate_hmap_pointer(                                     \
    (sizeof(**(map_ptr)) * (hmap_count)) + sizeof(HashMapHeader), "HashMap"   \
  );                                                                          \
  size_t *keys_idx_hmap = _instanciate_hmap_pointer(                          \
    (sizeof(size_t) * (hmap_count)), "HashMap Keys Idx Hmap"                  \
  );                                                                          \
  size_t *keys_idx = _instanciate_hmap_pointer(                               \
    (sizeof(size_t) * (hmap_count)), "HashMap Keys Idx"                       \
  );                                                                          \
  char **keys = _instanciate_hmap_pointer(                                    \
    (sizeof(char *) * (hmap_count)), "HashMap Keys"                           \
  );                                                                          \
  HashMapHeader *header = (HashMapHeader *)hmap;                              \
  header[0] = (HashMapHeader){                                                \
    .reserved_size=(hmap_count),                                              \
    .allocated=0,                                                             \
    .keys=keys,                                                               \
    .keys_idx=keys_idx,                                                       \
    .keys_idx_hmap=keys_idx_hmap,                                             \
  };                                                                          \
  (*(map_ptr)) = (void *)(header + 1);                                        \
} while(0)

/**
 * @brief Deallocates a hashmap's shadow header along with its key bookkeeping arrays.
 *
 * Locates the hidden HashMapHeader via reverse pointer arithmetic and releases the
 * keys_idx_hmap, keys_idx, keys arrays and the header block itself from the heap.
 * Does not reset the user's external pointer to NULL.
 *
 * @param map_ptr Pointer to the user's hashmap pointer targeted for destruction.
 */
#define __free_hmap_header(map_ptr) do {                                      \
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);                      \
  free(hmap_header->keys_idx_hmap);                                           \
  free(hmap_header->keys_idx);                                                \
  free(hmap_header->keys);                                                    \
  free(hmap_header);                                                          \
} while(0)

/**
 * @brief Expands the hashmap capacity using a dampening geometric growth formula and rehashes all entries.
 *
 * Internal routine that calculates a new slot count, allocates a fresh monolithic value block
 * plus new keys/keys_idx/keys_idx_hmap arrays, and re-inserts every existing key by recomputing
 * its hash against the new capacity (since the modulo used to derive the slot changes with size).
 * Frees the old header and its arrays before updating the user's external pointer.
 *
 * @param map_ptr Pointer to the user's hashmap pointer to receive the new data segment address.
 */
#define _dinamic_expand_hmap(map_ptr) do {                                    \
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);                      \
  size_t new_count = (                                                        \
    hmap_header->reserved_size +                                              \
    (hmap_header->reserved_size / 3) +                                        \
    (hmap_header->reserved_size >> 3) +                                       \
    (hmap_header->reserved_size < 9 ? 3 : 6)                                  \
  );                                                                          \
  size_t ptr_element_size = sizeof(**(map_ptr));                              \
  void *new_hmap = _instanciate_hmap_pointer(                                 \
    (ptr_element_size * new_count) + sizeof(HashMapHeader), "HashMap"         \
  );                                                                          \
  size_t *new_keys_idx_hmap = _instanciate_hmap_pointer(                      \
    (sizeof(size_t) * new_count), "HashMap Keys Idx"                          \
  );                                                                          \
  size_t *new_keys_idx = _instanciate_hmap_pointer(                           \
    (sizeof(size_t) * new_count), "HashMap Keys Idx"                          \
  );                                                                          \
  char **new_keys = _instanciate_hmap_pointer(                                \
    (sizeof(char *) * new_count), "HashMap Keys"                              \
  );                                                                          \
  HashMapHeader *new_header = (HashMapHeader *)new_hmap;                      \
  new_header[0] = (HashMapHeader){                                            \
    .reserved_size=new_count,                                                 \
    .allocated=hmap_header->allocated,                                        \
    .keys=new_keys,                                                           \
    .keys_idx=new_keys_idx,                                                   \
    .keys_idx_hmap=new_keys_idx_hmap,                                         \
  };                                                                          \
  new_hmap = (void *)(new_header + 1);                                        \
  for (size_t i=0; i < hmap_header->allocated; i++) {                         \
    size_t key_idx_by_arrive_order = hmap_header->keys_idx[i];                \
    char *key = hmap_header->keys[key_idx_by_arrive_order];                   \
    size_t new_hash = (hash_to_djb2((unsigned char *)(key)) % new_count);     \
    new_keys[new_hash] = key;                                                 \
    new_keys_idx_hmap[new_hash] = i;                                          \
    memcpy(                                                                   \
      (char *)new_hmap + (new_hash * ptr_element_size),                       \
      (char *)(* map_ptr) + (key_idx_by_arrive_order * ptr_element_size),     \
      ptr_element_size                                                        \
    );                                                                        \
    new_keys_idx[i] = new_hash;                                               \
    printf("%zu\n", new_hash);                                                \
  }                                                                           \
  __free_hmap_header(map_ptr);                                                \
  (*(map_ptr)) = new_hmap;                                                    \
} while(0)

/**
 * @brief Removes the key/value pair identified by the given key from the hashmap.
 *
 * Recomputes the key's hash to locate its slot, removes the value and key entries at
 * that slot, and removes the corresponding position from the arrival-order keys_idx array,
 * decrementing the logical usage counter.
 *
 * @param map_ptr Pointer to the user's hashmap pointer undergoing element deletion.
 * @param key The key (char *) identifying the entry to discard.
 */
#define hmapdel(map_ptr, key) do {                                                    \
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);                              \
  size_t hash = (hash_to_djb2((unsigned char *)(key)) % hmap_header->reserved_size);  \
  size_t keys_idx_pos = hmap_header->keys_idx_hmap[hash];                             \
  del_element(((void **)map_ptr), hash, sizeof(**(map_ptr)));                         \
  del_element((void **)&hmap_header->keys, hash, sizeof(char *));                     \
  del_element((void **)&hmap_header->keys_idx, keys_idx_pos, sizeof(size_t));         \
  del_element_and_resize(                                                             \
    (void **)&hmap_header->keys_idx,                                                  \
    keys_idx_pos,                                                                     \
    sizeof(size_t),                                                                   \
    hmap_header->allocated                                                            \
  );                                                                                  \
  hmap_header->allocated--;                                                           \
} while(0)

/**
 * @brief Builds a NULL-terminated array of the hashmap's keys, in insertion order, into dst.
 *
 * Aborts the program if dst is not NULL on entry. Otherwise, reads the hidden shadow header to
 * determine how many entries are populated, calloc's a char* buffer with room for that many keys
 * plus a NULL terminator slot, and walks keys_idx in arrival order to copy each corresponding
 * key reference into the buffer. Since this is a macro, the allocation is written straight into
 * dst through macro expansion, so dst must be declared as a modifiable pointer variable rather
 * than passed as an expression.
 *
 * @param map_ptr Pointer to the user's hashmap pointer to read keys from.
 * @param dst Pointer variable, passed as NULL, that will receive the allocated, NULL-terminated array of keys.
 */
#define get_hmap_keys(map_ptr, dst) do {                       \
  if (dst != NULL) {                                           \
    printf("Error on %s allocation!\n", "Hmap Keys ptr");      \
    exit(1);                                                   \
  }                                                            \
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);       \
  dst = calloc(hmap_header->allocated + 1, sizeof(char *));    \
  for (size_t i=0; i < hmap_header->allocated; i++) {          \
    size_t idx = hmap_header->keys_idx[i];                     \
    dst[i] = hmap_header->keys[idx];                           \
  }                                                            \
  dst[hmap_header->allocated + 1] = NULL;                      \
} while(0)

/**
 * @brief Inserts a key/value pair into the hashmap, or overwrites the value if the key already exists.
 *
 * Computes the key's hash to find its target slot. If the hashmap is at full capacity and the
 * key is not already present, triggers the dynamic capacity expansion routine before writing.
 * Records the key's arrival order in keys_idx/keys_idx_hmap and only increments the logical
 * usage counter when the key is new.
 *
 * @param map_ptr Pointer to the user's hashmap pointer where the entry should be written.
 * @param key The key (char *) identifying the entry.
 * @param value The raw value to store.
 */
#define hmap_insert(map_ptr, key, value) do {                                                   \
  HashMapHeader *hmap_header = get_hmap_header((map_ptr));                                      \
  size_t hash_key = (hash_to_djb2((unsigned char *)(key)) % hmap_header->reserved_size);        \
  bool already_exists_on_hmap = ((hmap_header->keys[hash_key]) != NULL);                        \
  if (hmap_header->allocated == hmap_header->reserved_size && !already_exists_on_hmap) {        \
    _dinamic_expand_hmap((map_ptr));                                                            \
    hmap_header = get_hmap_header((map_ptr));                                                   \
  }                                                                                             \
  hmap_header->keys_idx[hmap_header->allocated] = hash_key;                                     \
  hmap_header->keys_idx_hmap[hash_key] = hmap_header->allocated;                                \
  (*(map_ptr))[hash_key] = (value);                                                             \
  hmap_header->keys[hash_key] = (key);                                                          \
  if (!already_exists_on_hmap) {                                                                \
    hmap_header->allocated++;                                                                   \
  }                                                                                             \
} while(0)

/**
 * @brief Renders a full terminal visualization of an int-valued hashmap's header and key/value contents.
 *
 * Fetches the implicit shadow header object from the source hashmap address space, triggers
 * print_hmap_header(), and prints each entry as `key: value` following the insertion order
 * tracked by keys_idx.
 *
 * @param hmap_ptr The public hashmap data reference pointer to trace visually.
 */
#define print_hmap(hmap_ptr) do {                                             \
  HashMapHeader *header = get_hmap_header(hmap_ptr);                          \
  int *hmap = (*(hmap_ptr));                                                  \
                                                                              \
  print_hmap_header(&header);                                                 \
  printf("{");                                                                \
  for (size_t i=0; i < header->allocated; i++) {                              \
    size_t idx = header->keys_idx[i];                                         \
    printf("\n  %s: %d, ", header->keys[idx], hmap[idx]);                     \
  }                                                                           \
  if (header->allocated > 0) { printf("\n"); }                                \
  printf("}\n");                                                              \
} while (0)

/**
 * @brief Populates dst with a newly allocated array of the hashmap's values in insertion order.
 *
 * Requires dst to be NULL on entry, aborting the program otherwise. Reads the hidden shadow
 * header to determine how many entries are populated, calloc's a buffer sized to that count
 * times the element size, and walks keys_idx in arrival order to copy each corresponding value
 * into the buffer. As a macro, the allocated buffer is written straight into dst through macro
 * expansion (no dereference involved), so dst must be declared as a modifiable pointer variable
 * rather than passed as an expression.
 *
 * @param map_ptr Pointer to the user's hashmap pointer to read values from.
 * @param dst Pointer variable, passed as NULL, that will receive the allocated values buffer.
 */
#define get_hmap_values(map_ptr, dst) do {                     \
  if (dst != NULL) {                                           \
    printf("Error on %s allocation!\n", "Hmap Values ptr");    \
    exit(1);                                                   \
  }                                                            \
  HashMapHeader *hmap_header = get_hmap_header(map_ptr);       \
  size_t value_size = sizeof((**(map_ptr)));                   \
  dst = calloc(hmap_header->allocated, value_size);            \
  for (size_t i=0; i < hmap_header->allocated; i++) {          \
    size_t idx = hmap_header->keys_idx[i];                     \
    memcpy(                                                    \
      dst + (value_size * i),                                  \
      (* (map_ptr)) + (value_size * idx),                      \
      value_size                                               \
    );                                                         \
  }                                                            \
} while(0)

/**
 * @brief Prints out the raw structural metrics and key bookkeeping arrays tracked inside a HashMapHeader.
 *
 * Outputs current capacity limits (Reserved Space), real slots used (Allocated Data), and the
 * full contents of the keys_idx_hmap and keys_idx arrays.
 *
 * @param header_ptr Pointer to the HashMapHeader pointer to inspect.
 */
void print_hmap_header(HashMapHeader **header_ptr);

#endif
