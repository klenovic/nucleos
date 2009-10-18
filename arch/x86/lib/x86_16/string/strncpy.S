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

/* char* strncpy(char* dst, const char* src, uint32_t n) __attribute__((cdecl)) */
ENTRY16(strncpy)
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %edi	// get dst
	movl	16(%esp), %esi	// get src
	movl	20(%esp), %ecx	// get n
	movl	%edi, %edx	// save dst for further use
1:
	decl	%ecx		// decrement by 1
	js	2f		// sign?
	cld
	lodsb			// copy char
	stosb
	testb	%al,%al		// '\0'?
	jne	1b		// if not repeat copy
	rep			// fill the rest with '\0'
	stosb			//  i.e. dst must have enough space
2:
	movl	%edx, %eax	// return orig. dst value
	popl	%esi
	popl	%edi
	retl
END(strncpy)
