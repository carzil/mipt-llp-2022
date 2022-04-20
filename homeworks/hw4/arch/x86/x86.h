#pragma once

#include "types.h"

#define RFLAGS_IF (1<<9)

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

static inline void x86_hlt() {
    __asm__ volatile ("hlt");
}

static _Noreturn inline void x86_hlt_forever() {
    for (;;) {
        x86_hlt();
    }
}

static inline void x86_outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("out %0,%1" : : "a" (data), "d" (port));
}

typedef struct x86_tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_addr;
} __attribute__((packed)) x86_tss_t;
