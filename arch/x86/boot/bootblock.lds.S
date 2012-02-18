/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Linker script for bootblock code (512 bytes) */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH("i386")

#define ORIGIN      __ORIGIN__

SECTIONS
{
  /* text */
  .text 0 : AT (ADDR(.text) - ORIGIN) {
    *(.text.head)
    *(.text)
    *(.text.*)
  }

  /* read-only data */
  .rodata . : AT (ADDR(.rodata) - ORIGIN) {
    *(.rodata)  
    *(.rodata.*)
    *(.rom)
  }

  .data . : AT (ADDR(.data) - ORIGIN) {
    *(.data)
    *(.data.*)
  }

  .bss . : AT (ADDR(.bss) - ORIGIN) {
    *(.bss)
    *(.bss.*)
    *(COMMON)
  }
}
