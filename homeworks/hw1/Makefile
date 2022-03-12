AS=x86_64-elf-gcc
CC=x86_64-elf-gcc
LD=x86_64-elf-gcc
AS_FLAGS=-c -g
CC_FLAGS=-mno-mmx -mno-sse -mno-sse2 -fno-pie -c -g -mno-red-zone -std=gnu99 -ffreestanding -Wall -Wextra
LD_FLAGS=
OBJCOPY=objcopy
GRUB_MKRESCUE=grub-mkrescue

image: build
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub
	cp kernel.bin isodir/boot
	$(GRUB_MKRESCUE) -o kernel.iso isodir
	rm -rf isodir

build:
	$(AS) $(AS_FLAGS) boot.S -o boot.o
	$(CC) $(CC_FLAGS) vga.c -o vga.o
	$(CC) $(CC_FLAGS) kernel.c -o kernel.o
	$(CC) $(CC_FLAGS) multiboot.c -o multiboot.o
	$(CC) $(CC_FLAGS) printk.c -o printk.o
	$(CC) $(CC_FLAGS) panic.c -o panic.o
	$(LD) $(LD_FLAGS) -T linker.ld -ffreestanding -O2 -nostdlib boot.o vga.o printk.o panic.o multiboot.o kernel.o -o kernel.bin

clean:
	rm -f *.o
	rm -f kernel.iso
	rm -f kernel.bin

.PHONY: build clean
