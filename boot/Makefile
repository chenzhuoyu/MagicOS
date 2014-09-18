DD			= dd
NASM		= nasm -Ox
OBJCOPY		= objcopy

.PHONY: all clean copy

all: boot.img osldr

clean:
	rm -f *.o
	rm -f osldr osldr.elf
	rm -f boot.bin boot.img

boot.img: boot.bin
	$(DD) if=boot.bin of=boot.img bs=512 count=1
	$(DD) if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

boot.bin: boot.asm colors.inc puts_rm.inc
	$(NASM) -o boot.bin boot.asm

osldr: osldr.asm colors.inc puts_rm.inc page_table.inc
	$(NASM) -o osldr osldr.asm
