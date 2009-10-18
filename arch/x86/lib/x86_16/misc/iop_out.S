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
ENTRY16(outb)
	movw	4(%esp), %dx
	movb	8(%esp), %al
	outb	%al, %dx
	retl
END(outb)

/**
 * void outw(uint16_t p, uint16_t d) ATTR(cdecl);
 * @brief Copy 2-bytes to the IO port (noinline version).
 * @param p port
 * @param d data
 */
ENTRY16(outw)
	movw	4(%esp), %dx
	movw	8(%esp), %ax
	outw	%ax, %dx
	retl
END(outw)

/**
 * void outl(uint16_t p, uint32_t d) ATTR(cdecl);
 * @brief Copy 4-bytes to the IO port (noinline version).
 * @param p port
 * @param d data
 */
ENTRY16(outl)
	movw	4(%esp), %dx
	movl	8(%esp), %eax
	outl	%eax, %dx
	retl
END(outl)
