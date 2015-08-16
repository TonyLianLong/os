all:
	nasm init.asm -o init.o -d entry_address=`cat elfinfo.txt`
	python fill.py init.o 2560
	nasm fat.asm -o fat.bin
	python fill.py fat.bin 122880
	nasm root.asm -o root.bin
	python fill.py root.bin 16384
	cat init.o fat.bin > init.img
	cat fat.bin >> init.img
	cat root.bin >> init.img
	cp ../main main.o
	python fill.py main.o 32768
	python fill.py system 8192
	python fill.py child 8192
	cat main.o ASC12 system child >> init.img
	python fill.py init.img 1474560
asc12:
	rm ASC12
	cp ASC12_real ASC12
	python fill.py ASC12 8192