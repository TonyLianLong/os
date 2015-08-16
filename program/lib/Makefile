CFLAGS=-I ../include -fno-builtin
all:
	gcc -c api.c -masm=intel $(CFLAGS)
	nasm apiASM.asm -f elf -o apiASM.o
