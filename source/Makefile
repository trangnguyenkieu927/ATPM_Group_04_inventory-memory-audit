# Makefile for Inventory Management Program
CC = gcc
CFLAGS = -Wall -Wextra -I./include
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

SRC = $(wildcard src/*.c)
HDR = $(wildcard include/*.h)
OBJ = $(SRC:.c=.o)
TARGET = inventory_app
TEST_TARGET = inventory_tests
TEST_SRC = tests/test_inventory.c src/inventory.c src/file_io.c src/validation.c

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	powershell -Command "Remove-Item -Force -ErrorAction SilentlyContinue src/*.o, $(TARGET).exe, $(TEST_TARGET).exe"

test: $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)
	./$(TEST_TARGET)

sanitize-test: $(TEST_SRC)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) -o $(TEST_TARGET) $(TEST_SRC)
	./$(TEST_TARGET)

.PHONY: all clean test sanitize-test
