#include "types.h"
#include "mm/paging.h"
#include "early.h"
#include "arch/x86/x86.h"
#include "common.h"
#include "linker.h"

#define EARLY_TEXT __attribute__((section(".early.text")))
#define EARLY_BSS __attribute__((section(".early.bss,\"wa\",@nobits#")))

#define KERNEL_DIRECT_PHYS_MAPPING_SIZE_IN_GB (KERNEL_DIRECT_PHYS_MAPPING_SIZE / GB)
#define KERNEL_SECTIONS_SIZE_IN_GB            (KERNEL_SECTIONS_SIZE / GB)

// Provided by early.S.
extern pml4_t early_pml4;
extern uint32_t _multiboot_info_addr;

EARLY_BSS static pdpt_t direct_phys_mapping_pdpts[DIV_ROUNDUP(KERNEL_DIRECT_PHYS_MAPPING_SIZE_IN_GB, 512)] = {};
EARLY_BSS static pdpt_t higher_half_pdpt = {};
EARLY_BSS static pgdir_t higher_half_pgdirs[KERNEL_SECTIONS_SIZE_IN_GB] = {};
EARLY_BSS early_data_t early_data = {};

static EARLY_TEXT void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("out %0,%1" : : "a" (data), "d" (port));
}

static EARLY_TEXT void early_mask_pic_interrupts() {
    // Mask all interrupts in old PIC.
    outb(0x21, 0xff);
    outb(0xa1, 0xff);
}

// early_setup_paging prepares virtual memory for entering higher-half kernel code.
static EARLY_TEXT void early_setup_paging() {
    // 1. Setup direct physical memory mapping at KERNEL_DIRECT_PHYS_MAPPING_START.
    for (size_t i = 0; i < DIV_ROUNDUP(KERNEL_DIRECT_PHYS_MAPPING_SIZE_IN_GB, 512); i++) {
        uint64_t phys_addr = i * 512 * GB;
        uint64_t virt_addr = KERNEL_DIRECT_PHYS_MAPPING_START + phys_addr;
        for (size_t j = 0; j < 512; j++) {
            direct_phys_mapping_pdpts[i].entries[j] = phys_addr | PTE_PAGE_SIZE | PTE_PRESENT | PTE_WRITE;
            phys_addr += GB;
        }
        uint64_t pml4e = ((uint64_t)&direct_phys_mapping_pdpts[i]) | PTE_PRESENT | PTE_WRITE;
        early_pml4.entries[PML4E_FROM_ADDR(virt_addr)] = pml4e;
    }

    // 2. Setup higher-half kernel sections mapping.
    uint64_t virt_addr_start = KERNEL_SECTIONS_START;
    uint64_t virt_addr_curr = KERNEL_SECTIONS_START;
    uint64_t phys_addr_curr = (uint64_t)&_phys_start_hh;
    uint64_t phys_addr_end = (uint64_t)&_phys_end_kernel_sections;
    while (phys_addr_curr < phys_addr_end) {
        size_t pgdir_idx = PDPE_FROM_ADDR(virt_addr_curr) - PDPE_FROM_ADDR(virt_addr_start);
        higher_half_pgdirs[pgdir_idx].entries[PDE_FROM_ADDR(virt_addr_curr)] = phys_addr_curr | PTE_PAGE_SIZE | PTE_PRESENT | PTE_WRITE;
        phys_addr_curr += 2 * MB;
        virt_addr_curr += 2 * MB;
        higher_half_pdpt.entries[PDPE_FROM_ADDR(virt_addr_curr)] = ((uint64_t)&higher_half_pgdirs[pgdir_idx]) | PTE_PRESENT | PTE_WRITE;
    }
    early_pml4.entries[PML4E_FROM_ADDR(virt_addr_curr)] = ((uint64_t)&higher_half_pdpt) | PTE_PRESENT | PTE_WRITE;
}

extern void jump_to_kernel_main();

EARLY_TEXT void early_start() {
    early_mask_pic_interrupts();
    early_setup_paging();
    early_data.multiboot_info = PHYS_TO_VIRT(_multiboot_info_addr);
    jump_to_kernel_main();
}
