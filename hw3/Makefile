CC=gcc
ASM=yasm -f elf64

all: start.o libminic.o libminic.so

test%: start.o libminic.o libminic.so
	gcc -c -g -Wall -fno-stack-protector -fno-builtin $@.c
	gcc -nostdlib -o $@ $@.o start.o libminic.o
	gcc -nostdlib -o $@s $@.o start.o libminic.so

%.o: %.asm
	${ASM} $< -o $@

%.so: %.o
	ld -shared -o $@ $<

.PHONY: clean
clean:
	rm -f start.o libminic.o libminic.so
