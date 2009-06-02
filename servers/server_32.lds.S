/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#ifdef CONFIG_SERVERS_LINKAGE_SEPID
/* For servers linked with separated I&D */
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
#ifdef CONFIG_NULL_DEREFER
    /* Since plenty of legacy code does null pointer
       dereferencing (*NULL). This hack ensures that
       *NULL==0. Should be removed because it makes
       debugging harder. */
    LONG(0);
#endif
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

  .text . : AT (ADDR(.text) - LOAD_OFFSET) {
    __text = .;
    *(.text.head)
    KEEP(*(.text.init))
    *(.text)
    *(.text.*)
    . = ALIGN(PAGE_SIZE);
    __etext = .;
  }

  .rodata . : AT (ADDR(.rodata) - LOAD_OFFSET) {
    __data = .;
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
    __edata = .;
  }

  .bss . : AT (ADDR(.bss) - LOAD_OFFSET) {
    __bss = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(PAGE_SIZE);
    __ebss = .;
  }
  __end = .;
#endif

  /DISCARD/ :
  {
    *(.eh_frame)
    *(.comment)
    *(.comment.*)
    *(.note)
    *(.note.*)
  }
}
