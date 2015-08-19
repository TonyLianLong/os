LINKFILE = entry.o main.o asm.o
LINKFLAGS = -Ttext 0x9000
CFLAGS = -std=gnu99 -masm=intel -m64 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -Os
all:
	nasm entry.asm -o entry.o -f elf64
	gcc -S main.c -o main.asm $(CFLAGS) -Iinclude -fno-builtin -nostdlib
	nasm asm.asm -o asm.o -f elf64
	as main.asm -o main.o --64
	ld $(LINKFILE) -o main.elf $(LINKFLAGS) -e entry -melf_x86_64 -T link.ld -Os 
	objcopy -S -O binary main.elf main
	./elfinfosave main.elf ./img/elfinfo.txt
	make -C img
clean:
	make -C lib clean
	rm *.o main.asm main.elf main