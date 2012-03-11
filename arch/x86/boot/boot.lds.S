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

	.text		: { *(.text*) }

	. = ALIGN(16);
	.rodata		: { *(.rodata*) }

	. = ALIGN(16);
	.data		: { *(.data*) }

	. = ALIGN(16);
	.bss 		: {
		__bss_start = .;
		*(.bss)
		*(COMMON)
		__bss_end = .;
	}
	. = ALIGN(16);
	_end = .;

	/DISCARD/ : {
		*(*)
		*(.comment)
		*(.comment.*)
		*(.note)
		*(.note.*)
	}

	/*
	 * The ASSERT() sink to . is intentional, for binutils 2.14 compatibility:
	 */
	. = ASSERT(_end <= 0x8000, "Setup too big!");
	. = ASSERT(hdr == 0x1f1, "The setup header has the wrong offset!");

	/* Necessary for the very-old-loader check to work... */
	. = ASSERT(__end_init <= 5*512, "init sections too big!");
}
