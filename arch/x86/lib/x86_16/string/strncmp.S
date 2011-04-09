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

/* int32_t strncmp(const char *s1, const char *s2, uint32_t n) __attribute__((cdecl)) */
ENTRY16(strncmp)
	pushl	%edi
	pushl	%esi
	movl	12(%esp), %esi		// get s1
	movl	16(%esp), %edi		// get s2
	movl	20(%esp), %ecx		// get n
1:
	decl	%ecx
	js	2f	// was that zero?
	cld
	lodsb		// load byte into %al
	scasb		// compare
	jne 3f
	testb %al,%al	// '\0'?
	jne 1b		// if not repeat
2:
	xorl %eax,%eax	// return value 0
	jmp 4f
3:
	sbbl %eax,%eax	// substraction with borrow, %eax = %eax - (eax + CF)
			// CF was set according to comparation
	orb $1,%al	// return +1 or -1
4:
	popl %esi
	popl %edi
	retl
END(strncmp)
