#pragma once

#include "types.h"

static inline uint64_t x86_read_cr2() {
    uint64_t ret;
    __asm__ volatile (
        "mov %%cr2, %0"
        : "=r"(ret)
    );
    return ret;
}

static inline uint64_t x86_read_cr3() {
    uint64_t ret;
    __asm__ volatile (
        "mov %%cr3, %0"
        : "=r"(ret)
    );
    return ret;
}

static inline void x86_write_cr3(uint64_t x) {
    __asm__ volatile (
        "mov %0, %%cr3"
        : : "r"(x)
    );
}

static inline void x86_hlt_forever() {
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

static inline void x86_outb(uint16_t port, uint8_t data) {
    __asm__ volatile("out %0,%1" : : "a" (data), "d" (port));
}
