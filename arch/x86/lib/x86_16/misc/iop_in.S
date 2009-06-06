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
 * uint8_t inb(uint16_t p) ATTR(cdecl);
 * @brief Copy 1-byte from the IO port (noinline version).
 * @param p port
 * @return 1-byte data
 */
BEGIN_FUNC(inb,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  inb %dx, %al
  retl
END_FUNC(inb)

/**
 * uint16_t inw(uint16_t p)
 * @brief Copy 2-bytes from the IO port (noinline version).
 * @param p port
 * @return 2-bytes data
 */
BEGIN_FUNC(inw,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  inw %dx, %ax
  retl
END_FUNC(inb)

/**
 * uint32_t inl(uint16_t p)
 * @brief Copy 4-bytes from the IO port (noinline version).
 * @param p port
 * @return 4-bytes data
 */
BEGIN_FUNC(inl,P2ALIGN_16,0x90)
  movw 4(%esp), %dx
  inl %dx, %eax
  retl
END_FUNC(inb)
