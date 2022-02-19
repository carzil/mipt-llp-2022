.intel_syntax noprefix

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.set FIRSTMB,      0x100000
.set PML4_ADDR,    FIRSTMB
.set PDPT_ADDR,    FIRSTMB + 0x1000
.set GDT_64_ADDR,  FIRSTMB + 0x2000
.set STACK,        FIRSTMB + 0x4000

.set GDT_P, (1<<15)
.set GDT_S, (1<<12)
.set GDT_G, (1<<23)
.set GDT_L, (1<<21)

# .multiboot section defines Multiboot 2 compatible header.
.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.extern kernel_main

.section .text
    # _start is an entry point of our kernel, execution after bootloader starts here.
    .global _start
    .type _start, @function
    _start:
    # Tell assembler, that we are running 32-bit protected mode.
    .code32
        # To enable long mode, we need to setup basic paging.
        # Here we setup identity mapping for the first gigabyte.

        # Clear physical frames for PML4, PDPT and GDT.
        mov edi, FIRSTMB
        mov al, 0x0
        mov ecx, 3*0x1000
        rep stosb

        # PML4[0] = PDPT_addr | FLAG_PRESENT
        mov edi, PML4_ADDR
        mov dword ptr [edi], PDPT_ADDR | 1<<0

        # PDPT[0] = 0 | PTE_PAGE_SIZE | PTE_PRESENT
        mov edi, PDPT_ADDR
        mov dword ptr [edi], 1<<7 | 1<<0

        # Enabling 64-bit mode is described in ISDM, Volume 3A, Section 9.8.5.

        # Write PML4 physical address into CR3.
        mov eax, PML4_ADDR
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

        # == Setup GDT for 64-bit mode ==

        mov ebx, GDT_64_ADDR

        # Null segment.
        mov dword ptr [ebx], 0
        mov dword ptr [ebx + 4], 0
        add ebx, 8

        # 1: code segment.
        mov dword ptr [ebx], 0xFFFF
        mov dword ptr [ebx + 4], GDT_G | GDT_L | GDT_P | GDT_S | (1<<11) | (1<<9) | (0xF << 16)
        add ebx, 8

        # 2: TSS, for now it will be kind of magic.
        mov dword ptr [ebx], 0x00000068
        mov dword ptr [ebx + 4], 0x00CF8900
        add ebx, 8

        # GDT pointer.
        mov word ptr [ebx], 3*8
        mov dword ptr [ebx + 2], GDT_64_ADDR

        # Load new GDT.
        lgdt [GDT_64_ADDR + 3*8]

        ljmp 0x8:.ret
    .ret:
        # We we are now running 64-bit mode. Yay!
    .code64
        # Setup simple stack and call kernel_main.
        mov rsp, STACK
        call kernel_main


        # Halt processor, i.e. put it in a such state that next instruction will be executed only after interrupt.
        # Effectively it means inifinite non-busy loop.
    1:  hlt
        jmp 1b

    .size _start, . - _start
