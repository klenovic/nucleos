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
 * uint8_t* insb(uint16_t p, uint8_t* m);
 * @brief Copy 1-byte from the IO port to memory (noinline version).
 * @param p port
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(insb)
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %eax
	movl	%eax, %edi
	insb
	popl	%edi
	retl
END(insb)

/**
 * uint16_t* insw(uint16_t p, uint16_t* m);
 * @brief Copy 2-byte from the IO port to memory (noinline version).
 * @param p port
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(insw)
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %eax
	movl	%eax, %edi
	insw
	popl	%edi
	retl
END(insw)

/**
 * uint32_t* insl(uint16_t p, uint32_t* m);
 * @brief Copy 4-byte from the IO port to memory (noinline version).
 * @param p port
 * @param m memory location to store data
 * @return pointer to data
 */
ENTRY16(insl)
	pushl	%edi
	movw	8(%esp), %dx
	movl	12(%esp), %eax
	movl	%eax, %edi
	insl
	popl	%edi
	retl
END(insl)
