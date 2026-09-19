#include <stdio.h>

void test_array_and_header_should_initialize_correctly();
void test_array_should_append();
void test_array_should_insert();
void test_array_should_delete();
void test_array_index_of_should_return_correct_value();
void test_should_return_array_length();

void test_hashmap_and_header_should_initialize_correctly();
void test_hash_function_should_generate_same_value_to_same_input();

#define RUN_TEST(test_func) do {        \
  printf("[RUN] %s...\n", #test_func);  \
  test_func();                          \
  printf("      ✔ Passed!\n\n");        \
} while(0)

int main() {
  printf("\n======================================\n");
  printf("       RUNNING ARRAY TEST SUITE        \n");
  printf("======================================\n\n");

  RUN_TEST(test_array_and_header_should_initialize_correctly);
  RUN_TEST(test_array_should_append);
  RUN_TEST(test_array_should_insert);
  RUN_TEST(test_array_should_delete);
  RUN_TEST(test_array_index_of_should_return_correct_value);
  RUN_TEST(test_should_return_array_length);

  printf("\n========================================\n");
  printf("       RUNNING HASHMAP TEST SUITE        \n");
  printf("========================================\n\n");

  RUN_TEST(test_hashmap_and_header_should_initialize_correctly);
  RUN_TEST(test_hash_function_should_generate_same_value_to_same_input);

  printf("================================\n");
  printf("  SUCCESS: All tests passed!  \n");
  printf("================================\n\n");

  return 0;
}

