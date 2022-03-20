SHELL := /bin/bash

AS=x86_64-elf-gcc
CC=x86_64-elf-gcc
LD=x86_64-elf-gcc
OBJCOPY=x86_64-elf-objcopy
SED=sed
GRUB_MKRESCUE=grub-mkrescue

ASFLAGS=-g
CCFLAGS=-mno-mmx -mno-sse -mno-sse2 -fno-pie -g -mno-red-zone -std=gnu99 -ffreestanding -Wall -Wextra -nostdlib
LDFLAGS=-ffreestanding -nostdlib

C_SOURCES   := $(shell ls *.c)
ASM_SOURCES := $(shell ls *.S)
C_OBJS      := $(C_SOURCES:.c=.c.o)
ASM_OBJS    := $(ASM_SOURCES:.S=.S.o)

image: build
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp kernel.bin isodir/boot
	$(GRUB_MKRESCUE) -o kernel.iso isodir
	rm -rf isodir

build: .c-depend .S-depend $(C_OBJS) $(ASM_OBJS) linker.ld
	$(LD) $(LDFLAGS) -T <(cpp -P -E linker.ld) $(ASM_OBJS) $(C_OBJS) -o kernel.bin
	$(OBJCOPY) --only-keep-debug kernel.bin kernel.sym
	$(OBJCOPY) --strip-debug kernel.bin

%.c.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

.c-depend: $(C_SOURCES)
	rm -f ./.depend.c
	$(CC) $(CCFLAGS) -MM $^ > ./.c-depend
	$(SED) -i 's/^\(.*\)\.o/\1.c.o/g' .c-depend

.S-depend: $(ASM_SOURCES)
	rm -f ./.depend.S
	$(CC) $(CCFLAGS) -MM $^ > ./.S-depend
	$(SED) -i 's/^\(.*\)\.o/\1.S.o/g' .S-depend

clean:
	rm -f *.o
	rm -f .*-depend
	rm -f kernel.bin
	rm -f kernel.sym
	rm -f kernel.iso

.PHONY: build clean
