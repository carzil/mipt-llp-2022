SHELL := /bin/bash

C_SOURCES   := $(wildcard *.c)
ASM_SOURCES := $(wildcard *.S)
C_OBJS      := $(C_SOURCES:.c=.c.o)
ASM_OBJS    := $(ASM_SOURCES:.S=.S.o)

build: $(C_OBJS) $(ASM_OBJS)

%.c.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

%.S.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f kernel.bin
	rm -f kernel.sym
	rm -f kernel.iso

.PHONY: build clean
