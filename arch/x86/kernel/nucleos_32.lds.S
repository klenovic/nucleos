/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH("i386")

#include <asm/boot.h>
#include <asm/page_types.h>

#ifdef CONFIG_X86_32
#define LOAD_OFFSET	__PAGE_OFFSET
#endif

SECTIONS
{
	. = 0;
	_text = .;
	/* Future 16-bit real-mode kernel */
	.bstext		: { *(.bstext) }
	.bsdata		: { *(.bsdata) }

	. = 497;
	.header		: { *(.header) }
	.entrytext	: { *(.entrytext) }
	.inittext	: { *(.inittext) }
	.initdata	: { *(.initdata) }
	__end_init = .;

	/* The 32-bit kernel */
	. += LOAD_OFFSET + LOAD_PHYSICAL_ADDR;
	.text . : AT (ADDR(.text) - LOAD_OFFSET) {
		*(.text.head)
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

	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}
}
