CFLAGS = -I../include -std=gnu99 -masm=intel -Os
all:graphics.o interruptC.o interruptASM.o sys.o string.o timer.o keyboard.o \
taskC.o taskASM.o console.o floppyASM.o floppyC.o file.o elf.o RTC.o
%.o:%.c
	gcc -c $< $(CFLAGS) -fno-builtin -m64
%.o:%.asm
	nasm -f elf $< -o $(addsuffix .o,$(basename $<))
%.s:%.c
	gcc -S $< $(CFLAGS)
clean:
	rm *.o
