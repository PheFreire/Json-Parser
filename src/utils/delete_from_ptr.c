#include "utils/delete_from_ptr.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void del_element_and_resize(
  void **ref_to_ptr,
  size_t idx,
  size_t element_size,
  size_t ptr_lenght
) {
  uint8_t *ptr = (uint8_t *)(*(ref_to_ptr));
  
  uint8_t *dest = (ptr + ((idx) * (element_size)));
  uint8_t *src = (ptr + (((idx) + 1) * (element_size)));
  size_t size_to_move = (((ptr_lenght) - (idx) - 1) * (element_size));
  memmove(dest, src, size_to_move);
  
  uint8_t *ptr_to_last_element = (ptr + (((ptr_lenght) - 1) * (element_size)));
  memset(ptr_to_last_element, 0, (element_size));
}

void del_element(
  void **ref_to_ptr,
  size_t idx,
  size_t element_size
) {
  uint8_t *ptr = (uint8_t *)(*(ref_to_ptr));
  uint8_t *to_delete = (ptr + (idx * element_size));
  memset(to_delete, 0, element_size);
}
