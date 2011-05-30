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
  .text 0 : AT (ADDR(.text)) {
    __text = .;
    *(.text.head)
    *(.text)
    *(.text.*)
    . = ALIGN(0x10);
    __etext = SIZEOF(.text);
  }

  .data 0 : AT (SIZEOF(.text)) {
    __data = .;

    /* Since plenty of legacy code does null pointer
       dereferencing (*NULL). This hack ensures that
       *NULL==0. */
    LONG(0);

    *(.rodata)
    *(.rodata.*)
    *(.data)
    *(.data.*)
    . = ALIGN(0x10);
    __edata = SIZEOF(.data);
  }

  .bss . : AT (SIZEOF(.text) + SIZEOF(.data)) {
    __bss = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(0x10);
    __ebss = SIZEOF(.data) + SIZEOF(.bss);
  }
  __end = __ebss;

#else /* Common I&D */
  .text ORIGIN : AT (ADDR(.text) - ORIGIN) { /* Text */
    __text = . ;
    *(.text.head)
    *(.text)
    *(.text.*)
    . = ALIGN(0x10) ; /* round up the size of segment */
    __etext = . ;
  }

  .data . : AT (ADDR(.data) - ORIGIN) {
    __data = . ;
    *(.data)
    *(.data.*)
    *(.rodata) /* read-only data */
    *(.rodata.*)
    *(.rom)
    . = ALIGN(0x10) ;
    __edata = . ;
  }

  .bss . : AT (ADDR(.bss) - ORIGIN) {
    __bss = . ;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(0x10) ;
    __ebss = . ;
  }

  /* start of heap + stack */
  __end = . ;
#endif

  /DISCARD/ :
  {
    *(*)
    *(.comment)
    *(.comment.*)
    *(.note)
    *(.note.*)
  }
}
