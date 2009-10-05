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

/* _cpuid() - interface to cpuid instruction
 * void _cpuid(u32_t eax, u32_t *eax, u32_t *ebx, u32_t *ecx, u32_t *edx);
 */

.globl _cpuid
_cpuid:
	push	%ebp

	mov	%esp, %ebp

	// save work registers
	push	%eax
	push	%ebx
	push	%ecx
	push	%edx

	// set %eax parameter to cpuid and execute cpuid
	mov	24(%esp), %eax
	cpuid

	// store results in pointer arguments
	mov	28(%esp), %ebp
	mov	%eax, (%ebp)
	mov	32(%esp), %ebp
	mov	%ebx, (%ebp)
	mov	36(%esp), %ebp
	mov	%ecx, (%ebp)
	mov	40(%esp), %ebp
	mov	%edx, (%ebp)

	// restore registers
	pop	%edx
	pop	%ecx
	pop	%ebx
	pop	%eax

	pop	%ebp

	ret
