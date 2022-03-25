#pragma once

#define PAGE_SIZE   4096

#define CACHE_LINE_SIZE_BYTES 64

#define GB (1 << 30)
#define MB (1 << 20)
#define KB (1 << 10)

#define KERNEL_HIGHER_HALF_START 0xffff800000000000

#define KERNEL_SECTIONS_START    0xffffffff80000000
#define KERNEL_SECTIONS_SIZE     2 * (1ull << 30)

#define KERNEL_DIRECT_PHYS_MAPPING_START 0xffff888000000000
#define KERNEL_DIRECT_PHYS_MAPPING_SIZE  64 * (1ull << 40)
