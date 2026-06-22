# Variáveis de Compilador
CC = gcc
CFLAGS = -Wall -Wextra -Isrc  # -Isrc permite usar #include "pasta/arquivo.h"

# Localização de Arquivos
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/app

# Busca recursiva: encontra todos os arquivos .c dentro de src e subpastas
SRCS = $(shell find $(SRC_DIR) -name "*.c")

# Cria a lista de arquivos .o (ex: src/array/array.c vira build/array/array.o)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# --- REGRAS PRINCIPAIS ---

# 'make' ou 'make build' compila tudo
build: $(TARGET)

# 'make run' compila (se necessário) e executa
run: build
	./$(TARGET)

# Regra para criar o executável final
$(TARGET): $(OBJS)
	@echo "Linkando tudo no executável: $@"
	$(CC) $(OBJS) -o $(TARGET)

# Regra para compilar cada arquivo .c individualmente
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compilando: $<"
	@mkdir -p $(dir $@)          # Cria a subpasta dentro de build (ex: build/array/)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpa a pasta build
clean:
	rm -rf $(BUILD_DIR)

.PHONY: build run clean
