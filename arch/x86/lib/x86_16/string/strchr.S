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

/* char* arch_strchr(const char *s, int32_t c) __attribute__((cdecl)) */
ENTRY16(strchr)
	pushl	%esi
	movl	8(%esp), %esi	// get src
	movl	12(%esp), %eax	// get character
	movb	%al,%ah		// save for further check
1:
	cld
	lodsb			// load char into al
	cmpb	%ah,%al		// cmp with original
	je	2f
	testb	%al,%al		// zero?
	jne	1b		// if not then repeat
	movl	$1,%esi		// set 1 we will decrement this value by 1
2:
	movl	%esi,%eax	// return value
	decl	%eax		// this is zero or character
	popl	%esi
	retl
END(strchr)
