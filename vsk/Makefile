# Makefile
all: kernel.bin

kernel.bin: boot.o
	cat boot.o > kernel.bin
	dd if=/dev/zero bs=512 count=1 >> kernel.bin

boot.o: boot.asm
	nasm -f bin boot.asm -o boot.o

clean:
	rm -f kernel.bin boot.o

