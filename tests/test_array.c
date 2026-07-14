#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "array/array.h"

void test_array_and_header_should_initialize_correctly() {
  int *array = NULL;

  newmap(&array, 3);
  assert(array != NULL);
  assert(array[0] == 0);
  assert(array[1] == 0);
  assert(array[2] == 0);

  Header *header_ptr = get_header(array);
  assert(header_ptr != NULL);
  assert(header_ptr->reserved_size == 3);
  assert(header_ptr->allocated == 0);

  Header *header_ptr_2 = ((Header *)array) - 1;
  assert(header_ptr == header_ptr_2);
  
  freemap(&array);
  assert(array == NULL); 
}

void test_array_should_append() {
  int *array = NULL;
  newmap(&array, 3);

  Header *header_ptr = get_header(array);

  append(&array, 1);
  assert(array[0] == 1);
  assert(header_ptr->allocated == 1);
  assert(header_ptr->reserved_size == 3);

  append(&array, 2);
  assert(array[1] == 2);
  assert(header_ptr->allocated == 2);
  assert(header_ptr->reserved_size == 3);

  append(&array, 3);
  assert(array[2] == 3);
  assert(header_ptr->allocated == 3);
  assert(header_ptr->reserved_size == 3);

  append(&array, 4);
  header_ptr = get_header(array);

  assert(array[3] == 4);
  assert(header_ptr->allocated == 4);
  assert(header_ptr->reserved_size == 6);

  freemap(&array);
}

void test_array_should_insert() {
  int *array = NULL;
  newmap(&array, 6);

  append(&array, 1);
  append(&array, 2);
  append(&array, 3);

  Header *header_ptr = get_header(array);

  insert(&array, 1, 5);
  assert(array[0] == 1);
  assert(array[1] == 5);
  assert(array[2] == 2);
  assert(array[3] == 3);
  assert(header_ptr->allocated == 4);
  assert(header_ptr->reserved_size == 6);

  insert(&array, 2, 10);
  assert(array[0] == 1);
  assert(array[1] == 5);
  assert(array[2] == 10);
  assert(array[3] == 2);
  assert(array[4] == 3);
  assert(header_ptr->allocated == 5);
  assert(header_ptr->reserved_size == 6);

  insert(&array, 0, 21);
  assert(array[0] == 21);
  assert(array[1] == 1);
  assert(array[2] == 5);
  assert(array[3] == 10);
  assert(array[4] == 2);
  assert(array[5] == 3);
  assert(header_ptr->allocated == 6);
  assert(header_ptr->reserved_size == 6);

  insert(&array, 5, 22);
  assert(array[0] == 21);
  assert(array[1] == 1);
  assert(array[2] == 5);
  assert(array[3] == 10);
  assert(array[4] == 2);
  assert(array[5] == 22);
  assert(array[6] == 3);
  
  header_ptr = get_header(array);
  assert(header_ptr->allocated == 7);
  assert(header_ptr->reserved_size == 9);

  insert(&array, 10, 5);
  assert(array[0] == 21);
  assert(array[1] == 1);
  assert(array[2] == 5);
  assert(array[3] == 10);
  assert(array[4] == 2);
  assert(array[5] == 22);
  assert(array[6] == 3);
  assert(array[7] == 5);
  assert(header_ptr->allocated == 8);
  assert(header_ptr->reserved_size == 9);
  
  freemap(&array);
}

void test_array_should_delete() {
  int *array = NULL;
  newmap(&array, 3);

  Header *header_ptr = get_header(array);

  append(&array, 1);
  append(&array, 2);
  append(&array, 3);
  
  del(&array, 0);
  assert(array[0] == 2);
  assert(array[1] == 3);
  assert(header_ptr->allocated == 2);
  assert(header_ptr->reserved_size == 3);


  del(&array, 1);
  assert(array[0] == 2);
  assert(array[1] == 0);
  assert(header_ptr->allocated == 1);
  assert(header_ptr->reserved_size == 3);

  freemap(&array);
}

void test_ptr_array_should_delete() {
  char **array = NULL;
  newmap(&array, 3);

  Header *header_ptr = get_header(array);

  append(&array, "pato1");
  append(&array, "pato2");
  append(&array, "pato3");
  
  del_ptr(&array, 0);
  assert(strcmp(array[0], "pato2") == 0);
  assert(strcmp(array[1], "pato3") == 0);
  assert(header_ptr->allocated == 2);
  assert(header_ptr->reserved_size == 3);


  del_ptr(&array, 1);
  assert(strcmp(array[0], "pato2") == 0);
  assert(array[1] == NULL);
  assert(header_ptr->allocated == 1);
  assert(header_ptr->reserved_size == 3);

  freemap(&array);
}

void test_array_index_of_should_return_correct_value() {
  int *array = NULL;
  newmap(&array, 3);

  append(&array, 1);
  append(&array, 2);
  append(&array, 3);

  assert(index_of(&array, 1) == 0);
  assert(index_of(&array, 2) == 1);
  assert(index_of(&array, 3) == 2);
  assert(index_of(&array, 4) == -1);

  freemap(&array);
}

void test_should_return_array_length() {
  int *array = NULL;
  newmap(&array, 3);

  append(&array, 1);
  append(&array, 2);
  append(&array, 3);

  assert(maplen(&array) == 3);
  freemap(&array);
}

