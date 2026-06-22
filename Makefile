# Compiler Variables
CC = gcc
CFLAGS = -Wall -Wextra -Isrc  # -Isrc allows using #include "folder/file.h"

# File Locations
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
TARGET = $(BUILD_DIR)/app
TEST_TARGET = $(BUILD_DIR)/test_suite

# --- APPLICATION FILES (APP) ---
# Recursive search: finds all .c files inside src and its subfolders
SRCS = $(shell find $(SRC_DIR) -name "*.c")
# Creates the list of .o files (e.g., src/array/array.c becomes build/array/array.o)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# --- TEST MODULE FILES ---
# Filters and removes src/main.c so that tests use only the main from test_runner.c
SRC_FOR_TESTS = $(filter-out $(SRC_DIR)/main.c, $(SRCS))
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.c")
# List of objects required for linking tests (Production Code + Test Code)
TEST_OBJS = $(SRC_FOR_TESTS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) \
            $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/tests/%.o)

# --- MAIN RULES ---

# 'make' or 'make build' compiles everything for the main app
build: $(TARGET)

# 'make run' compiles (if necessary) and executes the main app
run: build
	./$(TARGET)

# 'make test' compiles everything related to tests and runs the runner
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Rule to create the final application executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to create the test suite executable
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET)

# Rule to compile each .c file from SRC individually
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling code: $<"
	@mkdir -p $(dir $@)          # Creates the subfolder inside build (e.g., build/array/)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile each .c file from TESTS individually
$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@echo "Compiling test: $<"
	@mkdir -p $(dir $@)          # Creates the build/tests/ subfolder
	$(CC) $(CFLAGS) -c $< -o $@

# Cleans the build folder
clean:
	rm -rf $(BUILD_DIR)

.PHONY: build run clean test

