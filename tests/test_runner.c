#include <stdio.h>

void test_array_and_header_should_initialize_correctly();
void test_array_should_append();
void test_array_should_insert();
void test_array_should_delete();
void test_array_index_of_should_return_correct_value();

#define RUN_TEST(test_func) do {          \
    printf("[RUN] %s...\n", #test_func);  \
    test_func();                          \
    printf("      ✔ Passed!\n\n");        \
} while(0)

int main() {
    printf("\n================================\n");
    printf("       RUNNING TEST SUITE        \n");
    printf("================================\n\n");

    RUN_TEST(test_array_and_header_should_initialize_correctly);
    RUN_TEST(test_array_should_append);
    RUN_TEST(test_array_should_insert);
    RUN_TEST(test_array_should_delete);
    RUN_TEST(test_array_index_of_should_return_correct_value);

    printf("================================\n");
    printf("  SUCCESS: All tests passed!  \n");
    printf("================================\n\n");

    return 0;
}

