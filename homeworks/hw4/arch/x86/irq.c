#include "irq.h"
#include "kernel/panic.h"
#include "sched/sched.h"
#include "arch/x86/x86.h"
#include "arch/x86/arch.h"
#include "drivers/apic.h"

void timer_handler() {
    apic_eoi();
    sched_timer_tick();
}

void spurious_handler() {
    apic_eoi();
}

#define PF_ERRCODE_P    (1<<0)
#define PF_ERRCODE_W    (1<<1)
#define PF_ERRCODE_U    (1<<2)
#define PF_ERRCODE_RSVD (1<<3)
#define PF_ERRCODE_I    (1<<4)
#define PF_ERRCODE_PK   (1<<5)

void pf_handler(arch_regs_t* ctx) {
    char reason[64];
    size_t pos = 0;

#define WR(f, msg) \
    if (ctx->errcode & (f)) { \
        const char* __msg = msg; \
        while (*__msg != '\0') { \
            reason[pos++] = *__msg; \
            __msg++; \
        } \
        reason[pos++] = ' '; \
    }

    WR(PF_ERRCODE_P, "P");
    WR(PF_ERRCODE_W, "W")
    WR(PF_ERRCODE_U, "U")
    WR(PF_ERRCODE_RSVD, "RSVD")
    WR(PF_ERRCODE_I, "I")
    WR(PF_ERRCODE_PK, "P")
#undef WR
    if (pos > 0) {
        reason[pos - 1] = '\0';
    }
    panic("pagefault [%s]: addr=%p, rip=%p", reason, x86_read_cr2(), ctx->rip);
}

void gp_handler(arch_regs_t* ctx) {
    panic("#GP(%x) at rip=%p", ctx->errcode, ctx->rip);
}

void np_handler(arch_regs_t* ctx) {
    panic("#NP(%x) at rip=%p", ctx->errcode, ctx->rip);
}

void ts_handler(arch_regs_t* ctx) {
    panic("#TS(%x) at rip=%p", ctx->errcode, ctx->rip);
}

void ss_handler(arch_regs_t* ctx) {
    panic("#SS(%x) at rip=%p", ctx->errcode, ctx->rip);
}

void df_handler(arch_regs_t* ctx) {
    panic("#DF(0) at rip=%p", ctx->errcode, ctx->rip);
}

void ud_handler(arch_regs_t* ctx) {
    panic("#UD(%x) at rip=%p", ctx->errcode, ctx->rip);
}

void nm_handler(arch_regs_t* ctx) {
    panic("#NM at rip=%p", ctx->rip);
}
