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
#ifdef CONFIG_BOOTMONITOR_LINKAGE_SEPID
/* For linkage type with separated I&D */
	_text = .;
	.text 0 : AT (ADDR(.text)) {
		*(.text.head)
		*(.text)
		*(.text.*)
		. = ALIGN(0x10);
	}
	_etext = SIZEOF(.text);

	_data = .;
	.data 0 : AT (SIZEOF(.text)) {
	/* Since plenty of legacy code does null pointer
	   dereferencing (*NULL). This hack ensures that
	   *NULL==0. */
		LONG(0);
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.data.*)
		. = ALIGN(0x10);
	}
	_edata = SIZEOF(.data);

	_bss = .;
	.bss . : AT (SIZEOF(.text) + SIZEOF(.data)) {
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(0x10);
	}
	_ebss = SIZEOF(.data) + SIZEOF(.bss);
	_end = _ebss;

#else /* Common I&D */
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
#endif
	/DISCARD/ : {
		*(*)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
