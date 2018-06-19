CC=gcc
CFLAGS=-Os -Wall

all: source.c
	${CC} ${CFLAGS} source.c -o lanchat
	strip lanchat

.PHONY: format
format:
	clang-format -style=Google -i source.c

.PHONY: clean
clean:
	rm -f lanchat
