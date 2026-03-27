CC = gcc
CFLAGS = -g -Og -Iinclude        # debug
# CFLAGS = -O2 -Iinclude         # release

TARGET = build/mkjava
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build/
