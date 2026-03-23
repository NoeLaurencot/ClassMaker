CC = gcc
CFLAGS = -O2 -Iinclude

TARGET = build/mkjava
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/
