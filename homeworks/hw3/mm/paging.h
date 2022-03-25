#pragma once

#include "defs.h"
#ifndef __ASSEMBLER__
#include <stddef.h>
#include <stdint.h>
#endif

#define PTE_PAGE_SIZE (1ull << 7)
#define PTE_PRESENT   (1ull << 0)

#define PTE_FLAGS_MASK ((1ull << 12) - 1)
#define PTE_ADDR_MASK  ((1ull << 48) - 1)
#define PTE_ADDR(pte) ((void*)(((pte) & PTE_ADDR_MASK) & ~PTE_FLAGS_MASK))

#define ATTR_ALIGN_4096 __attribute__((aligned(4096)))

#define PML4E_FROM_ADDR(addr) (((uint64_t)(addr) >> 39) & 0x1ff)
#define PDPE_FROM_ADDR(addr)  (((uint64_t)(addr) >> 30) & 0x1ff)
#define PDE_FROM_ADDR(addr)   (((uint64_t)(addr) >> 21) & 0x1ff)
#define PTE_FROM_ADDR(addr)   (((uint64_t)(addr) >> 12) & 0x1ff)

#define PHYS_TO_VIRT(addr) ((void*)(KERNEL_DIRECT_PHYS_MAPPING_START + (uint64_t)addr))
#define VIRT_TO_PHYS(addr) ((void*)((uint64_t)addr - KERNEL_DIRECT_PHYS_MAPPING_START))

#ifndef __ASSEMBLER__

struct pml4 {
    uint64_t entries[512];
} ATTR_ALIGN_4096;
typedef struct pml4 pml4_t;

struct pdpt {
    uint64_t entries[512];
} ATTR_ALIGN_4096;
typedef struct pdpt pdpt_t;

struct pgdir {
    uint64_t entries[512];
} ATTR_ALIGN_4096;
typedef struct pgdir pgdir_t;

struct pgtbl {
    uint64_t entries[512];
} ATTR_ALIGN_4096;
typedef struct pgtbl pgtbl_t;

#endif
