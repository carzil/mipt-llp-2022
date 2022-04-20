#include "irq.h"
#include "regs_asm.h"

.intel_syntax noprefix

.macro IDT_ENTRY vec entry flags
    // rbx = &IDT[vec]
    lea rbx, [rip + .Lidt + \vec*16]

    // rax = (0x08 << 16) | (entry & 0xFFFF)
    lea rax, [rip + _irq_entry_\entry]
    and eax, 0xFFFF
    or  eax, 0x08 << 16
    mov dword ptr [rbx], eax

    // rax = (entry & 0xFFFF0000) | (flags << 8)
    lea rax, [rip + _irq_entry_\entry]
    and eax, 0xFFFF0000
    or  eax, (\flags << 8)
    mov dword ptr [rbx + 4], eax

    // rax = entry >> 32
    lea rax, [rip + _irq_entry_\entry]
    shr rax, 32
    mov dword ptr [rbx + 8], eax

    mov dword ptr [rbx + 16], 0
.endm

.macro IRQ_ENTRY push_errcode entry
    .section .text
    .extern \entry
    .global _irq_entry_\entry
    .type _irq_entry_\entry, @function
    .align 16
_irq_entry_\entry:
    # Emulate error code on stack.
.ifeq \push_errcode
    pushq -1
.endif

    PUSH_REGS

    # Pass pointer for arch_regs_t* which is current stack top.
    mov rdi, rsp
    call \entry

    POP_REGS

    # Skip error code.
    add rsp, 8

    # Return from interrupt.
    iretq
.endm

.section .bss
    .Lidt:
        .zero 4*256

.section .data
    .Lidt_ptr:
        .word 4*256
        .quad .Lidt

.section .text
    IRQ_ENTRY push_errcode=1 entry=ud_handler
    IRQ_ENTRY push_errcode=1 entry=df_handler
    IRQ_ENTRY push_errcode=1 entry=ts_handler
    IRQ_ENTRY push_errcode=1 entry=np_handler
    IRQ_ENTRY push_errcode=1 entry=ss_handler
    IRQ_ENTRY push_errcode=1 entry=gp_handler
    IRQ_ENTRY push_errcode=1 entry=pf_handler
    IRQ_ENTRY push_errcode=0 entry=nm_handler
    IRQ_ENTRY push_errcode=0 entry=spurious_handler
    IRQ_ENTRY push_errcode=0 entry=timer_handler

    .global irq_init
    .type irq_init, @function
    irq_init:
        push rbp
        mov rbp, rsp

        IDT_ENTRY 6  ud_handler        GATE_INTERRUPT
        IDT_ENTRY 7  nm_handler        GATE_INTERRUPT
        IDT_ENTRY 8  df_handler        GATE_INTERRUPT
        IDT_ENTRY 10 ts_handler        GATE_INTERRUPT
        IDT_ENTRY 11 np_handler        GATE_INTERRUPT
        IDT_ENTRY 12 ss_handler        GATE_INTERRUPT
        IDT_ENTRY 13 gp_handler        GATE_INTERRUPT
        IDT_ENTRY 14 pf_handler        GATE_INTERRUPT
        IDT_ENTRY 32 timer_handler     GATE_INTERRUPT
        IDT_ENTRY 39 spurious_handler  GATE_INTERRUPT

        lidt [rip + .Lidt_ptr]

        mov rsp, rbp
        pop rbp
        ret
