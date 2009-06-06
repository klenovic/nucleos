/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>

/**
 * void outb(uint16_t p, uint8_t d) ATTR(cdecl);
 * @brief Copy 1-byte to the IO port (noinline version).
 * @param p port
 * @param d data
 */
BEGIN_FUNC(outb,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  movb 8(%esp), %al
  outb %al, %dx
  retl
END_FUNC(outb)

/**
 * void outw(uint16_t p, uint16_t d) ATTR(cdecl);
 * @brief Copy 2-bytes to the IO port (noinline version).
 * @param p port
 * @param d data
 */
BEGIN_FUNC(outw,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  movw 8(%esp), %ax
  outw %ax, %dx
  retl
END_FUNC(outw)

/**
 * void outl(uint16_t p, uint32_t d) ATTR(cdecl);
 * @brief Copy 4-bytes to the IO port (noinline version).
 * @param p port
 * @param d data
 */
BEGIN_FUNC(outl,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  movl 8(%esp), %eax
  outl %eax, %dx
  retl
END_FUNC(outl)
