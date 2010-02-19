/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// based on code by Philip Homburg <philip@cs.vu.nl>

.section .text
.globl __setjmp
__setjmp:
	movl	4(%esp), %eax		// jmp_buf
	movl	%edx, 28(%eax)		// save edx
	movl	0(%esp), %edx
	movl	%edx, 8(%eax)		// save program counter
	movl	%esp, 12(%eax)		// save stack pointer
	movl	%ebp, 16(%eax)		// save frame pointer
	movl	%ebx, 20(%eax)
	movl	%ecx, 24(%eax)
	movl	%esi, 32(%eax)
	movl	%edi, 36(%eax)
	
	movl	8(%esp), %edx		// save mask?
	movl	%edx, 0(%eax)		// save whether to restore mask
	testl	%edx, %edx
	jz		1f
	leal	4(%eax), %edx		// pointer to sigset_t
	pushl	%edx
	call	__newsigset		// save mask
	addl	$4, %esp
1:
	movl	$0, %eax
	ret
