#include "irq.h"
#include "kernel/panic.h"
#include "arch/x86.h"
#include "drivers/apic.h"

void timer_handler() {
    apic_eoi();
}

void spurious_handler() {
    apic_eoi();
}

void pf_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void gp_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void np_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void ts_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void ss_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void df_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void ud_handler(struct irqctx_errcode* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void nm_handler(struct irqctx* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}
