/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
.set SENDA, 16
.set SYSVEC, 33

.set MSGTAB, 8			// message table
.set TABCOUNT, 12			// number of entries in message table

.section .text

.globl _senda
_senda:
	push	%ebp
	mov	%esp,%ebp
	push	%ebx
	mov	TABCOUNT(%ebp), %eax 	// eax = count
	mov	MSGTAB(%ebp), %ebx  	// ebx = table
	mov	$SENDA, %ecx  		// _senda(table, count)
	int	$SYSVEC			// trap to the kernel
	pop	%ebx
	pop	%ebp
	ret
