CC = gcc
CFLAGS := -ffreestanding -m32 -O1 -fasm
CFLAGS += -g -std=c99 -Wall -Wextra -Wpedantic

QEMU := qemu-system-i386 -serial mon:stdio
KERN := kern2
BOOT := -kernel $(KERN) $(QEMU_EXTRA)
ASMS := $(wildcard *.S)  # wildcard *.S
SRCS := $(wildcard *.c) $(wildcard lib/*.c)  # wildcard *.c
OBJS := $(ASMS:%.S=%.o) $(SRCS:%.c=%.o) 

LIBS_DIR := lib
OBJS := $(patsubst $(LIBS_DIR)/%.o,%.o,$(OBJS))  # Le saco el "lib/" a los objetos .o de libs

GCC_PATH := /usr/lib/gcc/x86_64-linux-gnu/6
CFLAGS += -I$(GCC_PATH)/include -I$(GCC_PATH)/include-fixed
CFLAGS += -I$(LIBS_DIR)/

LIBGCC := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

ENTRY := _start

$(KERN): boot.o $(OBJS)
	ld -m elf_i386 -Ttext 0x100000 --entry $(ENTRY) $^ $(LIBGCC) -o $@
	# Verificar imagen Multiboot v1.
	grub-file --is-x86-multiboot $@

%.o: $(ASMS) $(SRCS)
	$(CC) $(CFLAGS) -c $^

.PHONY: clean
clean:
	rm -f $(KERN) *.o core

qemu: $(KERN)
	$(QEMU) $(BOOT)

qemu-gdb: $(KERN)
	$(QEMU) -kernel $(KERN) -S -gdb tcp:127.0.0.1:7508 $(BOOT)

gdb:
	gdb -q -s $(KERN) -n -ex 'target remote 127.0.0.1:7508'

.PHONY: qemu qemu-gdb gdb

# Sin esto me intenta compilar Makefile.o:
# gcc   Makefile.o   -o Makefile
.PHONY: Makefile
