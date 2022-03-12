#include "gdt.h"
#include "multiboot.h"
#include "paging.h"

.intel_syntax noprefix

# .multiboot section defines Multiboot 2 compatible header.
.section .multiboot
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_ARCH_32
    .long HEADER_LEN
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH_32 + HEADER_LEN)

    .word MULTIBOOT_TAG_END
    .word 0
    .long 8

    .set HEADER_LEN, . - .multiboot

# Section .bss contains zero-filled data. It doesn't appear in ELF file, but filled during image load.
.section .bss
    .Lbootstrap_stack:
        .zero 8192
    .Lpml4:
        .zero 4096
    .Lpdpt:
        .zero 4096

    .global _multiboot_info
    _multiboot_info:
        .long 0

# Section .data contains some global variables.
.section .data
    # Setup GDT for 64-bit mode.
    .Lgdt64:
        # 0: null segment.
        .long 0
        .long 0

        # 1: 64-bit kernel code segment.
        .long 0xFFFF
        # (0xF << 16) fills base 31:24.
        .long GDT_GRANULARITY | GDT_LONG | GDT_PRESENT | GDT_SYSTEM | (1<<11) | GDT_READ | (0xF << 16)

    .Lgdt64_ptr:
        .word . - .Lgdt64
        .long .Lgdt64

# kernel_main symbol is declared outside this file.
.extern kernel_main

.section .text
    # _start is an entry point of our kernel, execution after bootloader starts here.
    .global _start
    .type _start, @function
    _start:
    # Tell assembler, that we are running 32-bit protected mode.
    .code32
        # Our kernel just loaded, current processor state is described in "I386 machine state" section of Multiboot2 specification.

        mov _multiboot_info, ebx

        # To enable long mode, we need to setup basic paging.
        # Here we setup identity mapping for the first four gigabytes.

        # PML4[0] = PDPT_addr | FLAG_PRESENT
        lea edi, .Lpml4
        lea eax, .Lpdpt
        or eax, PTE_PRESENT
        mov dword ptr [edi], eax

        # PDPT[0] = 0 | PTE_PAGE_SIZE | PTE_PRESENT
        lea edi, .Lpdpt
        mov dword ptr [edi], PTE_PAGE_SIZE | PTE_PRESENT
        # PDPT[1] = (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT
        add edi, 8
        mov dword ptr [edi], (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT
        # PDPT[2] = 2 * (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT
        add edi, 8
        mov dword ptr [edi], 2 * (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT
        # PDPT[3] = 3 * (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT
        add edi, 8
        mov dword ptr [edi], 3 * (1 << 30) | PTE_PAGE_SIZE | PTE_PRESENT

        # Enabling 64-bit mode is described in ISDM, Volume 3A, Section 9.8.5.

        # Write PML4 physical address into CR3.
        lea eax, .Lpml4
        mov cr3, eax

        # Set PAE bit in CR4.
        mov eax, cr4
        or eax, 1<<5
        mov cr4, eax

        # Set IA32_EFER.LME bit (Long Mode Enable).
        mov ecx, 0xC0000080
        rdmsr
        or eax, 1<<8
        wrmsr

        # Enable paging by setting PG bit in CR0.
        mov eax, cr0
        or eax, 1<<31
        mov cr0, eax

        # Load GDT with 64-bit code segment.
        lgdt .Lgdt64_ptr

        # Load prepared code segment with index 1 with RPL=0 from GDT: segment selector is 0b1000 = 0x8.
        ljmp 0x8:.Lret
    .Lret:
        # We we are now running 64-bit mode. Yay!
    .code64
        # Load prepared data segments: segment selector is 0b10000 = 0x10.
        mov ax, 0
        mov ds, ax
        mov es, ax
        mov gs, ax
        mov fs, ax
        # Loading null segment in SS allowed only in 64-bit mode.
        mov ss, ax

        # Setup booststrap stack and call kernel_main.
        lea rsp, .Lbootstrap_stack
        call kernel_main

        # Halt processor, i.e. put it in a such state that next instruction will be executed only after interrupt.
        # Effectively it means inifinite non-busy loop.
    1:  hlt
        jmp 1b

    .size _start, . - _start
