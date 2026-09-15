# Makefile for Inventory Management Program
CC = gcc
CFLAGS = -Wall -Wextra -I./include

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = inventory_app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
