CC=gcc
CFLAGS=-O3 -Wall -static

source: source.c builtin.c
	cc ${CFLAGS} source.c builtin.c -o hw1

.PHONY: format
format:
	clang-format -style=Google -i *.c *.h

.PHONY: clean
clean:
	rm -f hw1
