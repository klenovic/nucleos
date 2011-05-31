/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* Note: This is exactly like using `--oformat BFDNAME' on the command line. If
	 both are used, the command line option takes precedence. */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH("i386")

#include <asm/page.h>

SECTIONS
{
#ifdef CONFIG_KERNEL_LINKAGE_SEPID
	_text = .;
	.text 0 : AT (ADDR(.text)) {
		*(.text.head)
		*(.text)
		*(.text.*)
		. = ALIGN(PAGE_SIZE);
	}
	_etext = SIZEOF(.text);

	_data = .;
	.data 0 : AT (SIZEOF(.text)) {

		SHORT(0x526f);					/* kernel data starts
								   with this magic number */
		. = (. == 0) ? PAGE_SIZE : ALIGN(PAGE_SIZE);	/* fist data page unmaped */
		*(.rodata)
		*(.rodata.*)
		*(.data)
		*(.data.*)
		. = ALIGN(PAGE_SIZE);
	}
	_edata = SIZEOF(.data);

	_bss = .;
	.bss . : AT (SIZEOF(.text) + SIZEOF(.data)) {
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(PAGE_SIZE);
	}
	_ebss = SIZEOF(.data) + SIZEOF(.bss);
	_end = _ebss;

#else /* Common I&D */

#define LOAD_OFFSET         __PAGE_OFFSET__
#define LOAD_PHYSICAL_ADDR  __LOAD_PHYSICAL_ADDR__

	. = LOAD_OFFSET + LOAD_PHYSICAL_ADDR;
	_text = .;
	.text.head . : AT (ADDR(.text.head) - LOAD_OFFSET) {
		*(.text.head)
		. = ALIGN(PAGE_SIZE);
	}

	.text . : AT (ADDR(.text) - LOAD_OFFSET) {
		*(.text)
		*(.text.*)
		. = ALIGN(PAGE_SIZE);
	}
	_etext = .;

	_data = .;
	.rodata . : AT (ADDR(.rodata) - LOAD_OFFSET) {
		_rodata = .;
		/* kernel data starts with this magic number */
		SHORT(0x526f);
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(PAGE_SIZE);
		_erodata = .;
	}

	.data . : AT (ADDR(.data) - LOAD_OFFSET) {
		*(.data)
		*(.data.*)
		. = ALIGN(PAGE_SIZE);
	}
	_edata = .;

	_bss = .;
	.bss . : AT (ADDR(.bss) - LOAD_OFFSET) {

		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(PAGE_SIZE);
	}
	_ebss = .;
	_end = _ebss;
#endif
	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
