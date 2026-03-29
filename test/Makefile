# Makefile for tests
# messy, just wanted a quick way to test functions

CC = gcc
CFLAGS = -Og -g -Wall -Wextra -I../include

SRC_DIR = ../src
BIN_DIR = bin

TEST_SRCS := $(wildcard test*.c)
SRC_SRCS := $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
TEST_BINS := $(patsubst %.c, $(BIN_DIR)/%, $(TEST_SRCS))

all: $(TEST_BINS)

$(BIN_DIR)/%: %.c $(SRC_SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -f $(TEST_BINS)