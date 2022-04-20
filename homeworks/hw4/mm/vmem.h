#pragma once

#include "paging.h"
#include "arch/x86/x86.h"

#define VMEM_NO_FLAGS 0
#define VMEM_USER     (1 << 0)
#define VMEM_WRITE    (1 << 1)

typedef struct vmem_area {
    void* start;
    size_t pgcnt;
    uint64_t flags;
    struct vmem_area* next;
} vmem_area_t;

// vmem_t is used to manage an address space, including all its page tables.
typedef struct vmem {
    pml4_t* pml4;
    vmem_area_t* areas_head;
} vmem_t;

// vmem_init_from_cr3 initializes vmem from current address space.
void vmem_init_from_current(vmem_t* vm);

// vmem_init_new initialized new vmem.
int vmem_init_new(vmem_t* vm);

// vmem_destroy destroys givem vmem and releases all allocated areas.
void vmem_destroy(vmem_t* vm);

// vmem_switch_to switches to the specified address space.
void vmem_switch_to(vmem_t* vm);

// vmem_alloc_pages allocates pgcnt frames and then maps it to the virtual address virt_addr.
int vmem_alloc_pages(vmem_t* vm, void* virt_addr, size_t pgcnt, uint64_t flags);

// vmem_map_1gb_page maps 1GB page to specified virt_addr.
int vmem_map_1gb_page(vmem_t* vm, void* virt_addr, void* phys_addr, uint64_t flags);

// vmem_map_2mb_page maps 2mb page to specified virt_addr.
int vmem_map_2mb_page(vmem_t* vm, void* virt_addr, void* phys_addr, uint64_t flags);

// vmem_map_page maps single page to specified virt_addr with given PTE flags.
int vmem_map_page(vmem_t* vm, void* virt_addr, void* phys_addr, uint64_t flags);

// vmem_copy_from_current copies all allocated areas from curr to dst, assuming that curr is an active address space.
int vmem_clone_from_current(vmem_t* dst, vmem_t* curr);
