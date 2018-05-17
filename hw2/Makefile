CC=gcc
CFLAGS=-Os -Wall

all: source.c
	${CC} ${CFLAGS} source.c -o hw2
	strip hw2

.PHONY: format
format:
	clang-format -style=Google -i source.c

.PHONY: clean
clean:
	rm -f hw2
