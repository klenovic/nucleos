/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/linkage.h>

/* char* strcat(char *dst, const char *src) __attribute__((cdecl)) */
ENTRY16(strcat)
	pushl	%edi
	pushl	%esi
	xorl	%eax, %eax
	orl	$-1, %ecx		// set max. value 
					//  (see repne termination condition)
	movl	16(%esp), %esi		// source
	movl	12(%esp), %edi		// destination
	movl	%edi, %edx
	cld
	repne	scasb			// find '\0' in string
					//  pointed by %edi
	decl	%edi
1:
	lodsb				// load byte from %esi
	stosb				// storre into %edi
	testb	%al, %al
	jne	1b
	movl	%edx, %eax		// return value
	popl	%esi
	popl	%edi
	retl
END(strcat)
