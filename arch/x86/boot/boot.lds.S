/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH("i386")

#define ORIGIN  __ORIGIN__

SECTIONS
{
	_text = . ;
	.text ORIGIN : AT (ADDR(.text) - ORIGIN) { /* Text */
		*(.text.head)
		*(.text)
		*(.text.*)
		. = ALIGN(0x10) ; /* round up the size of segment */
	}
	_etext = . ;

	_data = . ;
	.data . : AT (ADDR(.data) - ORIGIN) {
		*(.data)
		*(.data.*)
		*(.rodata) /* read-only data */
		*(.rodata.*)
		*(.rom)
		. = ALIGN(0x10) ;
	}
	_edata = . ;

	_bss = . ;
	.bss . : AT (ADDR(.bss) - ORIGIN) {
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(0x10) ;
	}
	_ebss = . ;

	/* start of heap + stack */
	_end = _ebss ;

	/DISCARD/ : {
		*(*)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
