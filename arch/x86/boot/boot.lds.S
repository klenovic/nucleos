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
	. = 0;
	_text = .;
	.bstext		: { *(.bstext) }
	.bsdata		: { *(.bsdata) }

	. = 497;
	.header		: { *(.header) }
	.entrytext	: { *(.entrytext) }
	.inittext	: { *(.inittext) }
	.initdata	: { *(.initdata) }
	__end_init = .;

	.text		: {
		*(.text)
		*(.text.*)
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
