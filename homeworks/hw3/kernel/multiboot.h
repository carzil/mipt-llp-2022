#pragma once

#define MULTIBOOT_TAG_END           0
#define MULTIBOOT_TAG_MMAP          6
#define MULTIBOOT_TAG_ACPI_OLD_RSDP 14

#define MULTIBOOT_MMAP_TYPE_RAM  1
#define MULTIBOOT_MMAP_TYPE_ACPI 3

#include "types.h"
#include "common.h"

struct multiboot_mmap_entry {
    u64 base_addr;
    u64 length;
    u32 type;
    u32 reserved;
};

struct multiboot_tag {
    u32 type;
    u32 size;
    char data[0];
};

struct multiboot_info {
    u32 total_size;
    u32 reserved;
    struct multiboot_tag first_tag;
};

struct multiboot_mmap_tag {
    struct multiboot_tag base;
    u32 entry_size;
    u32 entry_version;
    struct multiboot_mmap_entry first_entry;
};

// multiboot_init parses information provided by bootloader. It panics, if memory map tag found.
void multiboot_init();

struct multiboot_tag* multiboot_lookup_tag(uint32_t tag_code);

struct multiboot_mmap_iter {
    struct multiboot_mmap_entry* curr;
};

// multiboot_mmap_iter_init creates new iterator over memory map regions provided by bootloader.
void multiboot_mmap_iter_init(struct multiboot_mmap_iter* it);

// multiboot_mmap_iter_next should return next memory region from given iterator.
// If there are no more regions, return NULL instead.
struct multiboot_mmap_entry* multiboot_mmap_iter_next(struct multiboot_mmap_iter* it);

mem_region_t multiboot_mem_region();
