CC=gcc
CFLAGS=-g

source: source.c builtin.c
	cc ${CFLAGS} source.c builtin.c -o source

.PHONY: clean
clean:
	rm -f source
