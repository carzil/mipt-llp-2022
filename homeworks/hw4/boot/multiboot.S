#include "mm/paging.h"

#define MULTIBOOT_MAGIC    0xE85250D6
#define MULTIBOOT_ARCH_32  0

.intel_syntax noprefix

# .multiboot section defines Multiboot 2 compatible header.
.section .multiboot,"a"
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_ARCH_32
    .long HEADER_LEN
    .long -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH_32 + HEADER_LEN)

    // Tag end.
    .word 0
    .word 0
    .long 8

    .set HEADER_LEN, . - .multiboot
