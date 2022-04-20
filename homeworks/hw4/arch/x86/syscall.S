.intel_syntax noprefix

#include "gdt.h"
#include "regs_asm.h"

.extern _current
.extern do_syscall

.section .bss
    .Lsaved_rsp:
        .quad 0

# This is an entry point for syscall instruction.
# On enter, following holds:
#   rax contains syscall number;
#   rdi, rsi, rdx, rcx, r8, r9 contain syscall arguments (in order);
#   rcx contains userspace rip;
#   r11 contains userspace rflags;
#   rsp contains *userspace* stack (it may be corrupted or not mapped);
#   interrupts are disabled (IF set in IA32_FMASK).
.section .text
    .global syscall_entry
    .type syscall_entry, @function
    syscall_entry:
        # We cannot use user-controlled rsp here:
        # No stack switch will be performed if exception or interrupt occurs here since we are already in ring0.
        # So, invalid rsp leads us to the double fault.
        mov qword ptr [.Lsaved_rsp], rsp
        # rsp = _current->arch_thread.kstack_top
        mov rsp, qword ptr [_current]
        mov rsp, qword ptr [rsp]

        # We have a reliable stack now, enable interrupts.
        sti

        # TODO: construct arch_regs_t on stack and call do_syscall.


        # Restore user-space rsp.
        mov rsp, qword ptr [.Lsaved_rsp]

        sysretq

    .global pop_and_iret
    .type pop_and_iret, @function
    pop_and_iret:
        POP_REGS
        # Skip error code.
        add rsp, 8
        iretq
