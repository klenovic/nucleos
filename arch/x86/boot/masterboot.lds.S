/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Linker script for MBR code (512 bytes) */
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH("i386")

#define ORIGIN      __ORIGIN__
#define OFF_PARTTBL     0x01be
#define OFF_MAGIC       0x01fe
#define OFF_MINIXTBL     0x124

SECTIONS
{
  .mbr.begin ORIGIN : AT (ADDR(.mbr.begin) - ORIGIN) {
    KEEP (*(.mbr.begin))
  } = 0x90

  .text.head . : AT (ADDR(.text.head) - ORIGIN) {
    *(.text.head)
  }

  /* text */
  .text . : AT (ADDR(.text) - ORIGIN) {
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
#if 0
  . = OFF_MINIXTBL + ORIGIN;
  .minix.addr . : AT (ADDR(.minix.addr) - ORIGIN) {
    *(.minix.addr)
  }

  . = OFF_PARTTBL + ORIGIN;
  /* Partition table */
  .mbr.parttbl . : AT (ADDR(.mbr.parttbl) - ORIGIN) {
  *(.mbr.parttbl)
    /* size */
    . = 0x40;
  }

  /* Last word must be the magic word 0xaa55 */
  . = OFF_MAGIC + ORIGIN;
  .mbr.end . : AT (ADDR(.mbr.end) - ORIGIN) {
    KEEP(*(.mbr.end))
    BYTE(0x55);
    BYTE(0xaa);
    /* size */
    . = 0x02;
  }
#endif
}
