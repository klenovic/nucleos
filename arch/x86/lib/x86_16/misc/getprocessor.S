/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
//      getprocessor() - determine processor type
//      based on code by Kees J. Bot

#include <nucleos/linkage.h>

	.section ".text", "ax"

// int getprocessor(void);
//      Return 86, 186, 286, 386, 486, 586, ...
ENTRY16(getprocessor)
	push	%bp
	push	%bx
	mov	%sp, %bp
	push	%sp		// see if pushed sp == sp
	pop	%ax
	cmp	%sp, %ax
	jz	new_processor
	mov	$0x0120, %cx	// see if shifts are mod 32
	shlb	%cl, %ch	// zero tells if 86
	mov	$86, %ax
	jz	got_processor
	mov	$186, %ax
	jmp	got_processor

new_processor:			// see if high bits are set in saved IDT
	sub	$6, %sp		// space for IDT ptr
	sidt	-6(%bp)		// save 3 word IDT ptr
	cmpb	$0xFF, -1(%bp)	// top byte of IDT ptr is always FF on 286
	mov	$286, %ax
	je	got_processor

// 386, 486, 586
	and	$0xFFFC, %sp	// Align stack to avoid AC fault (needed?)
	mov	$0x0004, %cx	// Try to flip the AC bit introduced on the 486
	call	flip
	mov	$386, %ax	// 386 if it did not react to "flipping"
	jz	got_processor
	mov	$0x0020, %cx	// Try to flip the ID bit introduced on the 586
	call	flip
	mov	$486, %ax	// 486 if it did not react
	jz	got_processor
	pushf
	pusha			// Save the world
	xorl	%eax, %eax
	inc	%ax		// eax = 1
	cpuid
	andb	$0x0F, %ah	// Extract the family (5, 6, ...)
	movb	%ah, %al
	movb	$100, %ah
	mulb	%ah		// 500, 600, ...
	add	$86, %ax	// 586, 686, ...
	mov	%sp, %bx
	mov	%ax, 7*4(%bx)	// Pass ax through
	popa
	popf

got_processor:
	mov	%bp, %sp
	pop	%bx
	pop	%bp
	ret

flip:
	push	%bx		// Save bx and realign stack to multiple of 4
	pushf			// Push eflags
	pop	%ax
	pop	%dx		// dx:ax = eflags
	mov	%dx, %bx	// Save original eflags (high word only)
	xor	%cx, %dx	// Flip the bit to test
	push	%dx
	push	%ax		// Push modified eflags value
	popf			// Load modified eflags register
	pushf
	pop	%ax
	pop	%dx		// Get it again
	push	%bx
	push	%ax
	popf			// Restore original eflags register
	xor	%bx, %dx	// See if the bit changed
	test	%cx, %dx
	pop	%bx		// Restore bx
	ret
END(getprocessor)
