/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//       alloca() - allocate space on the stack
// based on code by Kees J. Bot

	.section ".text", "ax"

	.p2align  4
	.globl alloca
alloca:
	pop	%ecx             // Return address
	pop	%eax             // Bytes to allocate
	add	$3, %eax
	andb	$0xfc, %al       // Align
	sub	%eax, %esp       // Lower stack
	mov	%esp, %eax       // Return value
	push	%eax             // Dummy argument
	jmp	*%ecx
