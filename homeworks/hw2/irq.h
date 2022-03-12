#pragma once

#define GATE_INTERRUPT 0b10001110
#define GATE_TRAP      0b10001111

#ifndef __ASSEMBLER__

#include "types.h"

void irq_init();

static inline void irq_disable() {
    asm volatile ("cli");
}

static inline void irq_enable() {
    asm volatile ("sti");
}

// irqctx_errcode represent interrupt handler frame.
struct irqctx {
    // TODO: fill me.
};

// irqctx_errcode represent interrupt handler frame with error code.
struct irqctx_errcode {
    // TODO: fill me.
};

#endif
