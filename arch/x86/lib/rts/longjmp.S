/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
// based on code by Philip Homburg <philip@cs.vu.nl>

	.section ".text", "ax"
	.globl longjmp
longjmp:
	movl	4(%esp), %eax		// jmp_buf
	cmpl	$0, 0(%eax)			// save mask?
	je		1f
	leal	4(%eax), %edx		// pointer to sigset_t
	pushl	%edx
	call	__oldsigset		// restore mask
	addl	$4, %esp
	movl	4(%esp), %eax		// jmp_buf
1:	
	movl	8(%esp), %ecx		// result value
	movl	12(%eax), %esp 		// restore stack pointer

	movl	8(%eax), %edx 		// restore program counter
	movl	%edx, 0(%esp)

	pushl	%ecx			// save result code
	
	movl	16(%eax), %ebp		// restore frame pointer
	movl	20(%eax), %ebx
	movl	24(%eax), %ecx
	movl	28(%eax), %edx
	movl	32(%eax), %esi
	movl	36(%eax), %edi
	pop	%eax
	testl	%eax, %eax
	jz	1f
	ret
1:	movl	$1, %eax
	ret
