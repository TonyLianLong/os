CFLAGS = -fno-builtin -I ../include -masm=intel
all:
	make -C ../lib
	gcc -c child.c $(CFLAGS)
	ld child.o ../lib/api.o ../lib/apiASM.o -Ttext 0x0 -o child -e api_main
