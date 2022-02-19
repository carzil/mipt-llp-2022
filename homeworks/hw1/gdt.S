 .section .text
    .global load_gdt
    .type load_gdt, @function
load_gdt:
    movl    4(%esp), %eax
    lgdt	(%eax)

    # 0x10 is the data segment selector.
    movw	$0x10, %ax
    movw	%ax, %ds
    movw	%ax, %es
    movw	%ax, %fs
    movw	%ax, %gs
    movw	%ax, %ss

    # Load TSS.
    movw    $0x2b, %ax
    ltr     %ax

    ljmp	$0x08, $.flush  # Change CS.
.flush:
    ret

    .section .text
    .global userspace_fn
    .type userspace_fn, @function
userspace_fn:
    int $0x80
.l:
    jmp .l

    .section .text
    .global jump_userspace
    .type jump_userspace, @function
jump_userspace:
    mov     %esp, %eax
    push    $0x23          # Stack segement selector
    push    %eax           # ESP
    pushf                  # EFLAGS
    push    $0x1b          # Code segment selector
    push    $userspace_fn  # EIP

    movw	$0x23, %ax
    movw	%ax, %ds
    movw	%ax, %es
    movw	%ax, %fs
    movw	%ax, %gs
    iret
