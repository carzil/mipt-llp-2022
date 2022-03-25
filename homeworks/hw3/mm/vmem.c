#include "vmem.h"
#include "frame_alloc.h"
#include "paging.h"

static void vmem_map_page(vmem_t* vm, void* virt_addr, void* frame) {
    uint64_t pml4e = vm->pml4->entries[PML4E_FROM_ADDR(virt_addr)];
    pdpt_t* pdpt = NULL;
    if (!(pml4e & PTE_PRESENT)) {
        pdpt = frames_alloc(1);
        pml4e = vm->pml4->entries[PML4E_FROM_ADDR(virt_addr)] = (uint64_t)VIRT_TO_PHYS(pdpt) | PTE_PRESENT;
    } else {
        pdpt = PHYS_TO_VIRT(PTE_ADDR(pml4e));
    }

    uint64_t pdpe = pdpt->entries[PDPE_FROM_ADDR(virt_addr)];
    pgdir_t* pgdir = NULL;
    if (!(pdpe & PTE_PRESENT)) {
        pgdir = frames_alloc(1);
        pdpe = pdpt->entries[PDPE_FROM_ADDR(virt_addr)] = (uint64_t)VIRT_TO_PHYS(pgdir) | PTE_PRESENT;
    } else {
        pgdir = PHYS_TO_VIRT(PTE_ADDR(pdpe));
    }

    uint64_t pde = pgdir->entries[PDE_FROM_ADDR(virt_addr)];
    pgtbl_t* pgtbl = NULL;
    if (!(pde & PTE_PRESENT)) {
        pgtbl = frames_alloc(1);
        pdpe = pgdir->entries[PDE_FROM_ADDR(virt_addr)] = (uint64_t)VIRT_TO_PHYS(pgtbl) | PTE_PRESENT;
    } else {
        pgtbl = PHYS_TO_VIRT(PTE_ADDR(pde));
    }

    pgtbl->entries[PTE_FROM_ADDR(virt_addr)] = (uint64_t)VIRT_TO_PHYS(frame) | PTE_PRESENT;
}

void vmem_alloc_pages(vmem_t* vm, void* virt_addr, size_t pgcnt) {
    while (pgcnt > 0) {
        void* frame = frames_alloc(1);
        vmem_map_page(vm, virt_addr, frame);
        virt_addr += PAGE_SIZE;
        pgcnt--;
    }
}

void vmem_init_from_current(vmem_t* vm) {
    vm->pml4 = PHYS_TO_VIRT(x86_read_cr3());
}

void vmem_switch_to(vmem_t* vm) {
    x86_write_cr3((uint64_t)VIRT_TO_PHYS(vm->pml4));
}
