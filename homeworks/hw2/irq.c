#include "irq.h"
#include "panic.h"
#include "x86.h"
#include "apic.h"

void timer_handler() {
    printk(".");
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

void ud_handler(struct irqctx* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}

void nm_handler(struct irqctx* ctx) {
    // TODO: print information about context: error code, RIP and dump general-purpose registers.
}
