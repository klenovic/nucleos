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
 * void outsb(uint16_t p, uint8_t* m) ATTR(cdecl);
 * @brief Copy 1-byte from memory location to the IO port (noinline version).
 * @param p port
 * @param m pointer to data
 */
ENTRY16(outsb)
	pushl	%esi
	movw	8(%esp), %dx
	movl	12(%esp), %esi
	outsb
	popl	%esi
	retl
END(outsb)

/**
 * void outsw(uint16_t p, uint16_t* m) ATTR(cdecl);
 * @brief Copy 2-byte from memory location to the IO port (noinline version).
 * @param p port
 * @param m pointer to data
 */
ENTRY16(outsw)
	pushl	%esi
	movw	8(%esp), %dx
	movl	12(%esp), %esi
	outsw
	popl	%esi
	retl
END(outsw)

/**
 * void outsl(uint16_t p, uint32_t* m) ATTR(cdecl);
 * @brief Copy 4-byte from memory location to the IO port (noinline version).
 * @param p port
 * @param m pointer to data
 */
ENTRY16(outsl)
	pushl	%esi
	movw	8(%esp), %dx
	movl	12(%esp), %esi
	outsl
	popl	%esi
	retl
END(outsl)
