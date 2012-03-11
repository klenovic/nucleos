/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>

/**
 * uint8_t* blk_insb(uint16_t p, uint16_t count, uint8_t* m);
 * @brief Copy block from the IO port to memory. Data is read per bytes (noinline version).
 * @param p port
 * @param count number bytes to read
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(blk_insb)
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %ecx
	movl	16(%esp), %eax
	movl	%eax, %edi
	rep	insb
	popl	%edi
	retl
END(blk_insb)

/**
 * uint16_t* blk_insw(uint16_t p, uint16_t count, uint16_t* m) ATTR(cdecl);
 * @brief Copy block from the IO port to memory. Data is read per 2-bytes (noinline version).
 * @param p port
 * @param count number 2-bytes to read
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(blk_insw)
	pushl %edi
	movw	8(%esp), %dx
	movl	12(%esp), %ecx
	movl	16(%esp), %eax
	movl	%eax, %edi
	rep	insw
	popl	%edi
	retl
END(blk_insw)

/**
 * uint32_t* blk_insl(uint16_t p, uint16_t count, uint32_t* m) ATTR(cdecl);
 * @brief Copy block from the IO port to memory. Data is read per 4-bytes (noinline version).
 * @param p port
 * @param count number 4-bytes to read
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(blk_insl)
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %ecx
	movl	16(%esp), %eax
	movl	%eax, %edi
	rep	insl
	popl	%edi 
	retl
END(blk_insl)
