ENTRY(_start)

SECTIONS
{
	. = 2M;

    .multiboot : {
        KEEP(*(.multiboot))
    }

    .boot.text : {
        *(.boot.text)
    }

    .boot.bss : {
        *(.boot.bss)
    }

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
