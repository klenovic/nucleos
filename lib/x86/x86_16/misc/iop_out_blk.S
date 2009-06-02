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
 * void blk_outsb(uint16_t p, uint16_t count, uint8_t* m) ATTR(cdecl);
 * @brief Copy block of data from memory to the IO port. Data is written per bytes (noinline version).
 * @param p port
 * @param count number bytes to read
 * @param m pointer to data
 */
BEGIN_FUNC(blk_outsb,P2ALIGN_16,0x90)
  pushl %esi
  movw  8(%esp), %dx
  movl  12(%esp), %ecx
  movl  16(%esp), %esi
  rep   outsb
  popl  %esi
  retl
END_FUNC(blk_outsb)

/**
 * void blk_outsw(uint16_t p, uint16_t count, uint16_t* m) ATTR(cdecl);
 * @brief Copy block of data from memory to the IO port. Data is written per 2-bytes (noinline version).
 * @param p port
 * @param count number 2-bytes to read
 * @param m pointer to data
 */

BEGIN_FUNC(blk_outsw,P2ALIGN_16,0x90)
  pushl %esi
  movw  8(%esp), %dx
  movl  12(%esp), %ecx
  movl  16(%esp), %esi
  rep   outsw
  popl  %esi
  retl
END_FUNC(blk_outsw)

/**
 * void blk_outsl(uint16_t p, uint16_t count, uint32_t* m) ATTR(cdecl);
 * @brief Copy block of data from memory to the IO port. Data is written per 4-bytes (noinline version).
 * @param p port
 * @param count number 4-bytes to read 
 * @param m pointer to data
 */
BEGIN_FUNC(blk_outsl,P2ALIGN_16,0x90)
  pushl %esi
  movw  8(%esp), %dx
  movl  12(%esp), %ecx
  movl  16(%esp), %esi
  rep   outsl
  popl  %esi
  retl
END_FUNC(blk_outsl)
