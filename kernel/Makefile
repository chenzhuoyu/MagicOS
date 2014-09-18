NASM = nasm -Ox -f elf -I../include/
LD = ld -melf_i386 -s -nostdlib --no-undefined -Ttext=0x10400 -Map oskernel.map
CC = gcc -O3 -m32 -fno-builtin -fno-stack-protector -fpack-struct -falign-functions -std=gnu99 -I../include/ -c

ASM_OBJECTS =			\
	init.o				\
	irq.o				\
	restart.o

GCC_OBJECTS =			\
	global.o			\
	main.o				\
	stdlib.o			\
	stdio.o				\
	string.o			\
	vsprintf.o			\
	console.o			\
	i8259a.o			\
	interrupt.o			\
	i8042.o				\
	io.o				\
	slab.o				\
	thread.o			\
	sched.o				\
	syscall.o			\
	ki.o				\
	mm.o				\
	keyboard.o			\
	event.o				\
	semaphore.o			\
	ui.o

SOURCES = 				\
	init.asm			\
	irq.asm				\
	restart.asm			\
	global.c			\
	main.c				\
	stdlib.c			\
	stdio.c				\
	string.c			\
	vsprintf.c			\
	console.c			\
	i8259a.c			\
	interrupt.c			\
	i8042.c				\
	io.c				\
	slab.c				\
	thread.c			\
	sched.c				\
	syscall.c			\
	ki.c				\
	mm.c				\
	keyboard.c			\
	event.c				\
	semaphore.c			\
	ui.c

.PHONY: all clean

all: oskernel

clean:
	rm -f *.o
	rm -f *.c.d
	rm -f *.asm.d
	rm -f oskernel

$(GCC_OBJECTS): %.o : %.c
	$(CC) -o $@ $<

$(ASM_OBJECTS): %.o : %.asm
	$(NASM) -o $@ $<

oskernel: $(ASM_OBJECTS) $(GCC_OBJECTS)
	$(LD) -o oskernel $(ASM_OBJECTS) $(GCC_OBJECTS)

%.c.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $< > $@.tmp; \
	sed 's/\(^.*\)\.o: /\1.o $@: /g' < $@.tmp > $@; \
	rm -f $@.tmp

%.asm.d: %.asm
	@set -e; rm -f $@; \
	$(NASM) -o $*.o -M $< > $@.tmp; \
	sed 's/\(^.*\)\.o: /\1.o $@: /g' < $@.tmp > $@; \
	rm -f $@.tmp

include $(filter %.c.d,$(SOURCES:.c=.c.d))
include $(filter %.asm.d,$(SOURCES:.asm=.asm.d))