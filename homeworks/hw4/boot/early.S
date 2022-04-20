#include "arch/x86/gdt.h"
#include "mm/paging.h"

.intel_syntax noprefix

# Section .early.bss contains zero-filled data. It doesn't appear in ELF file, but filled during image load.
.section .early.bss,"wa",@nobits
    .align PAGE_SIZE
    .Lbootstrap_stack_bottom:
        .zero 4*PAGE_SIZE
    bootstrap_stack_top:

    .global early_pml4
    .type early_pml4, @object
    .align PAGE_SIZE
    early_pml4:
        .zero PAGE_SIZE

    .align PAGE_SIZE
    .Learly_pdpt:
        .zero PAGE_SIZE

    .global _multiboot_info_addr
    _multiboot_info_addr:
        .long 0

# Section .data contains some global variables.
.section .early.data,"wa"
    # Setup GDT for 64-bit mode.
    .Lgdt64:
        # 0: null segment.
        .long 0
        .long 0

        # 1: 64-bit kernel code segment.
        .long 0xFFFF
        # (0xF << 16) fills base 31:24.
        .long GDT_GRANULARITY | GDT_LONG | GDT_PRESENT | GDT_SYSTEM | GDT_CODE_SEG | GDT_READ | (0xF << 16)

    .Lgdt64_ptr:
        .word . - .Lgdt64
        .quad .Lgdt64

.macro CLEAR start size
    mov esi, \start
    mov al, 0
    mov ecx, \size
    rep stosb
.endm

.section .early.text
    # _early_entry_point is an entry point of our kernel, execution after bootloader starts here.
    .global _early_entry_point
    .type _early_entry_point, @function
    _early_entry_point:
    # Tell assembler, that we are running 32-bit protected mode.
    .code32
        # Our kernel just loaded, current processor state is described in "I386 machine state" section of Multiboot2 specification.
        mov _multiboot_info_addr, ebx

        # To enable long mode, we need to setup basic paging.
        # Here we setup identity mapping for the first four gigabytes.

        CLEAR early_pml4 PAGE_SIZE
        CLEAR .Learly_pdpt PAGE_SIZE

        # PML4[0] = PDPT_addr | FLAG_PRESENT
        lea edi, early_pml4
        lea eax, .Learly_pdpt
        or eax, PTE_PRESENT
        mov dword ptr [edi], eax

        # PDPT[0] = 0 | PTE_PAGE_SIZE | PTE_PRESENT
        lea edi, .Learly_pdpt
        mov dword ptr [edi], PTE_PAGE_SIZE | PTE_PRESENT

        # 64-bit mode enabling process is described in ISDM, Volume 3A, Section 9.8.5.

        # Write PML4 physical address into CR3.
        lea eax, early_pml4
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
        lea rsp, bootstrap_stack_top
        call early_start

    .extern kernel_main
    .extern kernel_main_return
    .extern early_data

    .global jump_to_kernel_main
    .type jump_to_kernel_main, @function
    jump_to_kernel_main:
        # Because KERNEL_DIRECT_PHYS_MAPPING_START is not representable as imm32, we need move it to register first.
        mov rbx, KERNEL_DIRECT_PHYS_MAPPING_START

        # Reset to the same stack, but use its virtual address instead.
        lea rsp, bootstrap_stack_top
        add rsp, rbx
        mov rbp, rsp

        # If kernel_main occasionally returns, kernel_main_return will be called.
        lea rax, kernel_main_return
        push rax

        # Use movabs to indicate that we want use imm64.
        lea rax, kernel_main
        lea rdi, early_data
        add rdi, rbx
        jmp rax
