#include "irq.h"

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

#define ERRCODE 1
#define NOERRCODE 0

.macro IRQ_ENTRY errcode entry
    .section .text
    .extern \entry
    .global _irq_entry_\entry
    .type _irq_entry_\entry, @function
    .align 16
_irq_entry_\entry:
    # First of all, save GPRs on stack.
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    # Pass pointer for struct irqctx* which is current stack top.
    mov rdi, rsp
    call \entry

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

.if \errcode
    # Skip error code.
    add rsp, 8
.endif

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
    IRQ_ENTRY ERRCODE   ud_handler
    IRQ_ENTRY ERRCODE   df_handler
    IRQ_ENTRY ERRCODE   ts_handler
    IRQ_ENTRY ERRCODE   np_handler
    IRQ_ENTRY ERRCODE   ss_handler
    IRQ_ENTRY ERRCODE   gp_handler
    IRQ_ENTRY ERRCODE   pf_handler
    IRQ_ENTRY NOERRCODE nm_handler
    IRQ_ENTRY NOERRCODE spurious_handler
    IRQ_ENTRY NOERRCODE timer_handler

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
