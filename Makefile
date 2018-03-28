CC=gcc
CFLAGS=-g

source: source.c builtin.c
	cc ${CFLAGS} source.c builtin.c -o source

.PHONY: format
format:
	clang-format -style=Google -i *.c *.h

.PHONY: clean
clean:
	rm -f source
