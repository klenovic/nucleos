/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

.section .text

.globl i386_invlpg

//===========================================================================
//                              i386_invlpg
//===========================================================================
// void i386_invlpg(u32_t addr)
// Tell the processor to invalidate a tlb entry at virtual address addr.
i386_invlpg:
	push	%ebp
	mov	%esp, %ebp
	push	%eax

	mov	8(%ebp), %eax
	invlpg	(%eax)

	pop	%eax
	pop	%ebp
	ret
