ENTRY(_early_entry_point)

#include "defs.h"

SECTIONS
{
    . = SIZEOF_HEADERS;

    .multiboot : ALIGN(4) {
        KEEP(*(.multiboot))
    }

    . = 2M;

    _phys_start_kernel_sections = .;
    .early.text : {
        *(.early.text)
    }

    .early.rodata : {
        *(.early.rodata)
    }

    .early.data : {
        *(.early.data)
    }

    .early.bss : {
        *(.early.bss)
    }

    _phys_start_user = ALIGN(PAGE_SIZE);

    . = 0x10000;

    .user.text : AT(_phys_start_user) {
        *(.user.text)
    }
    _phys_end_user = _phys_start_user + . - 0x10000;

    . = _phys_end_user;
    . = ALIGN(0x200000);
    _phys_start_hh = .;

    . = KERNEL_SECTIONS_START;
    .text : AT(_phys_start_hh + ADDR(.text) - KERNEL_SECTIONS_START) {
        *(.text)
    }

    .rodata : AT(_phys_start_hh + ADDR(.rodata) - KERNEL_SECTIONS_START) {
        *(.rodata)
    }

    .data : AT(_phys_start_hh + ADDR(.data) - KERNEL_SECTIONS_START) {
        *(.data)
    }

    .bss : AT(_phys_start_hh + ADDR(.bss) - KERNEL_SECTIONS_START) {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(0x200000);
    PROVIDE(_phys_end_kernel_sections = _phys_start_hh + . - KERNEL_SECTIONS_START);

    /DISCARD/ : {
        *(.eh_frame)
        *(.note.gnu.*)
        *(.dynamic)
    }
}
