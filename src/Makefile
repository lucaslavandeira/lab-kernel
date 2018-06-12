CFLAGS := -ffreestanding -m32 -O1 -fasm
CFLAGS += -g -std=c99 -Wall -Wextra -Wpedantic

QEMU := qemu-system-i386 -serial mon:stdio
KERN := kern0
BOOT := -kernel $(KERN)
ASMS := $(wildcard *.S)  # wildcard *.S
SRCS := $(wildcard *.c)  # wildcard *.c
OBJS := $(ASMS:%.S=%.o) $(SRCS:%.c=%.o) 

ENTRY := _start
kern0: boot.o $(OBJS)
	ld -m elf_i386 -Ttext 0x100000 --entry $(ENTRY) $^ -o $@
	# Verificar imagen Multiboot v1.
	grub-file --is-x86-multiboot $@

%.o:
	$(CC) $(CFLAGS) -c $(ASMS) $(SRCS)

clean:
	rm -f kern0 *.o core

qemu: $(KERN)
	$(QEMU) $(BOOT)

qemu-gdb: $(KERN)
	$(QEMU) -kernel kern0 -S -gdb tcp:127.0.0.1:7508 $(BOOT)

gdb:
	gdb -q -s kern0 -n -ex 'target remote 127.0.0.1:7508'

.PHONY: qemu qemu-gdb gdb

.PHONY: clean

