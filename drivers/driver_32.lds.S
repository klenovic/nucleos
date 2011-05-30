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

#include <asm/page.h>

ENTRY(_start)

SECTIONS
{
#ifdef CONFIG_UKERNEL_LINKAGE_SEPID
/* For drivers linked with separated I&D */
	.text 0 : AT (ADDR(.text)) {
		__text = .;
		*(.text.head)
		KEEP(*(.text.init))
		*(.text)
		*(.text.*)
		. = ALIGN(PAGE_SIZE);
		__etext = SIZEOF(.text);
	}

	.data 0 : AT (SIZEOF(.text)) {
		__data = .;
		. = (. == 0) ? (PAGE_SIZE) : ALIGN(PAGE_SIZE);  /* fist data page unmaped */
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.data.*)
		. = ALIGN(PAGE_SIZE);
		__edata = SIZEOF(.data);
	}

	.bss . : AT (SIZEOF(.text) + SIZEOF(.data)) {
		__bss = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(PAGE_SIZE);
		__ebss = SIZEOF(.data) + SIZEOF(.bss);
	}

	__end = __ebss;

#else /* Common I&D */

#define LOAD_OFFSET         __PAGE_OFFSET__
#define LOAD_PHYSICAL_ADDR  __LOAD_PHYSICAL_ADDR__

	. = LOAD_OFFSET + LOAD_PHYSICAL_ADDR;

	__text = .;
	.text . : AT (ADDR(.text) - LOAD_OFFSET) {
		*(.text.head)
		KEEP(*(.text.init))
		*(.text)
		*(.text.*)
		. = ALIGN(PAGE_SIZE);
	}
	__etext = .;

	__data = .;
	.rodata . : AT (ADDR(.rodata) - LOAD_OFFSET) {
		__rodata = .;
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(PAGE_SIZE);
		__erodata = .;
	}

	.data . : AT (ADDR(.data) - LOAD_OFFSET) {
		*(.data)
		*(.data.*)
		. = ALIGN(PAGE_SIZE);
	}
	__edata = .;

	__bss = .;
	.bss . : AT (ADDR(.bss) - LOAD_OFFSET) {

		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(PAGE_SIZE);
	}
	__ebss = .;
	__end = __ebss;
#endif

	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
