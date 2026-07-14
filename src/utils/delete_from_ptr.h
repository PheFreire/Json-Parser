#ifndef DELETE_FROM_PTR
#define DELETE_FROM_PTR

#include <stddef.h>

void del_element_and_resize(
  void **ref_to_ptr,
  size_t idx,
  size_t element_size,
  size_t ptr_lenght
);

void del_element(
  void **ref_to_ptr,
  size_t idx,
  size_t element_size
);

#endif

