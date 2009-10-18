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

/* int32_t strcmp(const char *s1, const char *s2) __attribute__((cdecl)) */
ENTRY16(strcmp)
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %esi	// get s1
	movl	16(%esp), %edi	// get s2
1:
	cld
	lodsb			// load char into %al
	scasb			// compare (%edi) with %al
	jne 2f			// if not equal jmp to 2 (ZF==0)
	testb	%al, %al	// zero?
	jne	1b		// if not repeat scan else
	xorl	%eax, %eax	// return value 0
	jmp	3f		// jump to end
2:
	sbbl	%eax, %eax	// %eax = %eax - (%eax + CF)
				//  CF was set according comparation
	orb	$1,%al		// return +1 or -1
3:
	popl	%esi
	popl	%edi
	retl
END(strcmp)
