# Makefile for Inventory Management Program
CC ?= gcc
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -I./include
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

SRC = $(wildcard src/*.c)
HDR = $(wildcard include/*.h)
OBJ = $(SRC:.c=.o)
TARGET = inventory_app
TEST_TARGET = inventory_tests
TEST_SRC = tests/test_inventory.c src/inventory.c src/product.c src/file_io.c src/validation.c src/utils.c
SMOKE_TARGET = smoke_tests
SMOKE_SRC = tests/smoke_test.c src/inventory.c src/product.c src/file_io.c src/validation.c src/utils.c src/report.c

ifeq ($(OS),Windows_NT)
    TARGET_BIN = $(TARGET).exe
    TEST_BIN = $(TEST_TARGET).exe
    SMOKE_BIN = $(SMOKE_TARGET).exe
    CLEAN_CMD = powershell -NoProfile -Command "Get-ChildItem -Path src/*.o, *.exe, $(TARGET), $(TEST_TARGET), $(SMOKE_TARGET), data/test_products_output.txt, data/test_smoke_products.txt, data/test_smoke_report.txt, data/test_legacy_4field.txt -ErrorAction SilentlyContinue | Remove-Item -Force; exit 0"
else
    TARGET_BIN = $(TARGET)
    TEST_BIN = $(TEST_TARGET)
    SMOKE_BIN = $(SMOKE_TARGET)
    CLEAN_CMD = rm -f src/*.o $(TARGET) $(TEST_TARGET) $(SMOKE_TARGET) *.exe data/test_products_output.txt data/test_smoke_products.txt data/test_smoke_report.txt data/test_legacy_4field.txt
endif

all: $(TARGET)

debug: CFLAGS += -g -O0
debug: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(CLEAN_CMD)

test: $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)
	./$(TEST_TARGET)

smoke-test: $(SMOKE_SRC)
	$(CC) $(CFLAGS) -o $(SMOKE_TARGET) $(SMOKE_SRC)
	./$(SMOKE_TARGET)

sanitize-test: $(TEST_SRC)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) -o $(TEST_TARGET) $(TEST_SRC)
	./$(TEST_TARGET)

.PHONY: all debug clean test smoke-test sanitize-test
