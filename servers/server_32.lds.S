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
#define LOAD_OFFSET		__PAGE_OFFSET__
#define LOAD_PHYSICAL_ADDR	__LOAD_PHYSICAL_ADDR__

	. = LOAD_OFFSET + LOAD_PHYSICAL_ADDR;
	_text = .;
	.text . : AT (ADDR(.text) - LOAD_OFFSET) {
		*(.text.head)
		KEEP(*(.text.init))
		*(.text)
		*(.text.*)
		. = ALIGN(PAGE_SIZE);
	}
	_etext = .;

	_data = .;
	.rodata . : AT (ADDR(.rodata) - LOAD_OFFSET) {
		_rodata = .;
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

	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
