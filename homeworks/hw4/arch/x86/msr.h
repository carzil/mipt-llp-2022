#pragma once

#include <stdint.h>

#define IA32_EFER     0xc0000080
#define IA32_EFER_SCE (1<<0)

#define IA32_STAR  0xc0000081
#define IA32_LSTAR 0xc0000082
#define IA32_CSTAR 0xc0000083
#define IA32_FMASK 0xc0000084

#define IA32_FS_BASE        0xc0000100
#define IA32_GS_BASE        0xc0000101
#define IA32_KERNEL_GS_BASE 0xc0000102

void x86_wrmsr(uint32_t msr, uint64_t x) {
    uint32_t hi = (uint32_t)(x >> 32);
    uint32_t lo = x & 0xffffffff;
    __asm__ volatile (
        "movl %0, %%ecx\n"
        "movl %1, %%eax\n"
        "movl %2, %%edx\n"
        "wrmsr"
        :
        : "r"(msr), "r"(lo), "r"(hi)
        : "eax", "ecx", "edx"
    );
}

uint64_t x86_rdmsr(uint32_t msr) {
    uint32_t lo;
    uint32_t hi;
    __asm__ volatile (
        "movl %2, %%ecx\n"
        "rdmsr\n"
        : "=a"(lo), "=d"(hi)
        : "r"(msr)
        : "ecx"
    );
    uint64_t res = hi;
    return (res << 32) | lo;
}
