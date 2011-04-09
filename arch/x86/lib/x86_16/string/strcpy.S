/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>

/* char* strcpy(char* dst, const char* src) __attribute__((cdecl)) */
ENTRY16(strcpy)
	pushl	%edi
	pushl	%esi
	movl	16(%esp), %esi	// get src
	movl	12(%esp), %edi	// get dst
	movl	%edi, %edx	// save dst
1:
	cld
	lodsb			// load byte
	stosb			// set byte
	testb	%al,%al		// '\0'?
	jne	1b		// repeat if not
	movl	%edx, %eax	// set %eax = dst
	popl	%esi
	popl	%edi
	retl
END(strcpy)
