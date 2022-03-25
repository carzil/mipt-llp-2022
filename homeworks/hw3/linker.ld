ENTRY(_early_entry_point)

#include "defs.h"

SECTIONS
{
    // https://stackoverflow.com/questions/15400910/ld-linker-script-producing-huge-binary
    . = SIZEOF_HEADERS;

    .multiboot : ALIGN(4) {
        KEEP(*(.multiboot))
    }

    . = 2M;

    PROVIDE(_phys_start_kernel_sections = .);
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

    . = ALIGN(0x200000);
    _end_early = .;

    . = KERNEL_SECTIONS_START;
    _phys_start_hh = _end_early + . - KERNEL_SECTIONS_START;
    .text : AT(_end_early + ADDR(.text) - KERNEL_SECTIONS_START) {
        *(.text)
    }

    .rodata : AT(_end_early + ADDR(.rodata) - KERNEL_SECTIONS_START) {
        *(.rodata)
    }

    .data : AT(_end_early + ADDR(.data) - KERNEL_SECTIONS_START) {
        *(.data)
    }

    .bss : AT(_end_early + ADDR(.bss) - KERNEL_SECTIONS_START) {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(0x200000);
    PROVIDE(_phys_end_kernel_sections = _end_early + . - KERNEL_SECTIONS_START);

    /DISCARD/ : {
        *(.eh_frame)
        *(.note.gnu.*)
        *(.dynamic)
    }
}
