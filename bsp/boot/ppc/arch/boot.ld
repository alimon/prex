/*
 * Linker script for boot loader
 */
INCLUDE config.ld
OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc)

SECTIONS {
	. = CONFIG_LOADER_TEXT ;

	.text . : {
		*(.text)
	}
	. = ALIGN(4);
	.rodata . : {
		*(.rodata)
		*(.rodata.*)
	}
	. = ALIGN(4);
	.data . : {
		*(.data)
		*(.sdata)
		*(.sdata2)
	}
	. = ALIGN(4);
	.bss . : {
		*(.sbss)
		*(.bss)
		*(COMMON)
	}
	.tail : {
		FILL(0xDEADC0DE);
        . = ALIGN(CONFIG_LOADER_MAX_SIZE) - 1;
        BYTE(0x00);
	}
}
