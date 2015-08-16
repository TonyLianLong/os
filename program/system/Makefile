CFLAGS = -fno-builtin -I ../include
all:
	make -C ../lib
	gcc -c system.c $(CFLAGS)
	ld system.o ../lib/api.o ../lib/apiASM.o -Ttext 0x0 -o system -e api_main
