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

/* void* memset(void* dst, uint8_t c, uint32_t n) __attribute__((cdecl)) */
ENTRY16(memset)
	pushl	%edi
	movl	8(%esp), %edx	// get destination address
	movl	16(%esp), %ecx	// get count
	testl	%ecx, %ecx
	je	1f
	xorl	%eax, %eax
	movb	12(%esp), %al	// get character
	testb	%al,%al		// check whether zero
	je	2f
	mov	%al, %ah	// repeat input character
	mov	%ax, %di	//  input 
	shll	$16,%eax	//  character
	mov	%di,%ax		//  in %eax
2:
	shrl	$2, %ecx	// set per 4-byte
	mov	%edx, %edi	// going to use destination
	cld
	rep	stosl
	movl	16(%esp), %ecx	// count
	andl	$3, %ecx
	rep	stosb		// copy the rest per byte
1:
	mov	%edx, %eax	// return value
	popl	%edi
	retl
END(memset)
