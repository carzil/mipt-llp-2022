#include "x86.h"
#include "gdt.h"
#include "msr.h"
#include "arch.h"
#include "mm/paging.h"
#include "kernel/irq.h"
#include "kernel/errno.h"
#include "mm/frame_alloc.h"
#include "common.h"
#include "irq.h"

static x86_tss_t tss;

typedef struct x86_gdt_descriptor {
    uint32_t dw0;
    uint32_t dw1;
} __attribute__((packed)) x86_gdt_descriptor_t;

#define GDT_DESCRIPTOR(idx, base, limit, flags) gdt[idx] = (x86_gdt_descriptor_t){                           \
    .dw0 = (((base) & 0xFFFF) << 16) | (limit & 0xFFFF),                                \
    .dw1 = ((((base) >> 16) & 0xFF)) | (flags) | GDT_PRESENT | ((((limit) >> 16) & 0xF) << 16) | ((((base) >> 24) & 0xFF) << 24),  \
}

#define GDT_DESCRIPTOR_64(idx, flags) GDT_DESCRIPTOR(idx, 0, 0xFFFFF, flags)

#define TSS_DESCRIPTOR_64(idx, base, limit, flags) \
    GDT_DESCRIPTOR(idx, base, limit, flags); \
    gdt[idx + 1] = (x86_gdt_descriptor_t){.dw0 = (base >> 32) & 0xFFFFFFFF, .dw1 = 0}

#define MAX_GDT_DESCRIPTORS 16

x86_gdt_descriptor_t gdt[MAX_GDT_DESCRIPTORS] = {};

typedef struct x86_gdt_pointer {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) x86_gdt_pointer_t;

static void gdt_init() {
    // 64-bit kernel code segment.
    GDT_DESCRIPTOR_64(1, GDT_GRANULARITY | GDT_LONG | GDT_SYSTEM | GDT_CODE_SEG | GDT_READ);
    // Data ring0 segment.
    GDT_DESCRIPTOR_64(2, GDT_GRANULARITY | GDT_SYSTEM | GDT_READ);

    // Data ring3 segment.
    GDT_DESCRIPTOR_64(3, GDT_GRANULARITY | GDT_SYSTEM | GDT_DPL_RING3 | GDT_READ);
    // 64-bit ring3 code segment.
    GDT_DESCRIPTOR_64(4, GDT_GRANULARITY | GDT_LONG | GDT_SYSTEM | GDT_DPL_RING3 | GDT_CODE_SEG | GDT_READ);

    // TSS descriptor, occupies 2 GDT entries.
    TSS_DESCRIPTOR_64(5, (uint64_t)&tss, sizeof(tss), GDT_GRANULARITY | (1<<20) | ((0b1001) << 8) | GDT_PRESENT);

    // Load new GDT.
    x86_gdt_pointer_t ptr = { .size = sizeof(gdt), .base = (uint64_t)&gdt, };
    __asm__ volatile (
        "lgdtq (%0)"
        :
        : "r"(&ptr)
    );
}

static void load_tss() {
    __asm__ volatile (
        "mov %0, %%ax\n"
        "ltr %%ax\n"
        :
        : "i"(GDT_SEGMENT_SELECTOR(TSS_SEG, RPL_RING0))
    );
}

extern void syscall_entry();

static void syscall_init() {
    // Enable syscall/sysret instructions.
    uint64_t efer = x86_rdmsr(IA32_EFER);
    x86_wrmsr(IA32_EFER, efer | IA32_EFER_SCE);

    // Syscall entry RIP.
    x86_wrmsr(IA32_LSTAR, (uint64_t)syscall_entry);

    // Mask interrupts on syscall enter.
    x86_wrmsr(IA32_FMASK, RFLAGS_IF);

    // Here is a tricky moment.
    // SYSRET loads predefined values in CS and SS. Despite that GDT is not referenced at this stage,
    // new segment selectors must correspond to valid descriptors,
    // otherwise IRET from interrupt will lead to #GP due to invalid segment selector.
    // SYSRET assumes that user code segment follows user data segment:
    // * CS = IA32_STAR[63:48] + 16
    // * SS = IA32_STAR[63:48] + 8
    uint64_t star = (uint64_t)GDT_SEGMENT_SELECTOR(KERNEL_CODE_SEG, RPL_RING0) << 32;
    star |= (uint64_t)GDT_SEGMENT_SELECTOR(USER_DATA_SEG - 1, RPL_RING3) << 48;
    x86_wrmsr(IA32_STAR, star);
}

extern void irq_init();

void arch_init() {
    // After entering higher-half code, GDT needs to be relocated as well.
    gdt_init();
    load_tss();
    syscall_init();
    irq_init();
}

void arch_thread_switch(arch_thread_t* prev, arch_thread_t* next) {
    tss.rsp0 = (uint64_t)next->kstack_top;
    context_switch(&prev->context, &next->context);
}

static int allocate_kstack(arch_thread_t* th) {
    th->kstack_top = frame_alloc();
    if (th->kstack_top == NULL) {
        return -ENOMEM;
    }
    memset(th->kstack_top, '\0', PAGE_SIZE);
    th->kstack_top += PAGE_SIZE;
    return 0;
}

extern void pop_and_iret();
extern void user_program();

int arch_thread_new(arch_thread_t* th, arch_regs_t** result_regs) {
    int err = allocate_kstack(th);
    if (err < 0) {
        return err;
    }

    uint8_t* kstack_top = th->kstack_top;
    kstack_top -= sizeof(arch_regs_t);
    arch_regs_t* regs = (arch_regs_t*)kstack_top;
    regs->rsp = 0x70000000 + 4*PAGE_SIZE;
    regs->ss = GDT_SEGMENT_SELECTOR(USER_DATA_SEG, RPL_RING3);
    regs->rip = (uint64_t)user_program;
    regs->cs = GDT_SEGMENT_SELECTOR(USER_CODE_SEG, RPL_RING3);
    regs->rflags = RFLAGS_IF;
    if (result_regs != NULL) {
        *result_regs = regs;
    }

    kstack_top -= sizeof(on_stack_context_t);
    on_stack_context_t* onstack_ctx = (on_stack_context_t*)kstack_top;
    onstack_ctx->ret_addr = (uint64_t)pop_and_iret;
    th->context.rsp = (uint64_t)kstack_top;
    return 0;
}

int arch_thread_clone(arch_thread_t* dst, arch_regs_t** regs, arch_thread_t* src) {
    UNUSED(src);

    int err = allocate_kstack(dst);
    if (err < 0) {
        return err;
    }

    uint8_t* kstack_top = dst->kstack_top;
    kstack_top -= sizeof(arch_regs_t);
    if (regs != NULL) {
        *regs = (arch_regs_t*)kstack_top;
    }

    kstack_top -= sizeof(on_stack_context_t);
    on_stack_context_t* onstack_ctx = (on_stack_context_t*)kstack_top;
    onstack_ctx->ret_addr = (uint64_t)pop_and_iret;
    dst->context.rsp = (uint64_t)kstack_top;
    return 0;
}

void arch_thread_destroy(arch_thread_t* th) {
    frame_free(th->kstack_top - PAGE_SIZE);
}
