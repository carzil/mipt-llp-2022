#pragma once

#define MULTIBOOT_MAGIC    0xE85250D6
#define MULTIBOOT_ARCH_32  0

#define MULTIBOOT_TAG_END  0
#define MULTIBOOT_TAG_MMAP 6

#define MULTIBOOT_MMAP_TYPE_RAM  1
#define MULTIBOOT_MMAP_TYPE_ACPI 3


#ifndef __ASSEMBLER__

#include "types.h"


struct multiboot_mmap_entry {
    u64 base_addr;
    u64 length;
    u32 type;
    u32 reserved;
};

// multiboot_init parses information provided by bootloader. It panics, if memory map tag found.
void multiboot_init();

struct multiboot_mmap_iter {
};

// multiboot_mmap_iter_init creates new iterator over memory map regions provided by bootloader.
void multiboot_mmap_iter_init(struct multiboot_mmap_iter* it);

// multiboot_mmap_iter_next should return next memory region from given iterator.
// If there are no more regions, return NULL instead.
struct multiboot_mmap_entry* multiboot_mmap_iter_next(struct multiboot_mmap_iter* it);

#endif
