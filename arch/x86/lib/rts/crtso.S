/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* This is the C run-time start-off routine.  It's job is to take the
 * arguments as put on the stack by EXEC, and to parse them and set them up the
 * way _main expects them.
 * It also initializes _environ when this variable isn't defined by the
 * programmer.  The detection of whether _environ belong to us is rather
 * simplistic.  We simply check for some magic value, but there is no other
 * way.
 */
.section .text.init
	.p2align 12
.section .text
.globl _start
_start:
	xor	%ebp, %ebp		// clear for backtrace of core files
	mov	(%esp), %eax		// argc
	lea	4(%esp), %edx		// argv
	lea	8(%esp,%eax,4), %ecx	// envp

	/* Test if environ is in the initialized data area and is set to our
	 * magic number. If so then it is not redefined by the user.
	 */
	mov	$environ, %ebx
	cmp	$__edata, %ebx		// within initialized data?
	jae	0f
	testb	$3, %bl			// aligned?
	jnz	0f
	cmpl	$0x53535353, (%ebx)	// is it our _environ?
	jne	0f
	mov	%ebx, _penviron		// _penviron = &environ;
0:
	mov	_penviron, %ebx
	mov	%ecx, (%ebx)		// *_penviron = envp;

	push	%ecx	// push envp
	push	%edx	// push argv
	push	%eax	// push argc

	// Test the EM bit of the MSW to determine if an FPU is present and
	// set _fpu_present if one is found.
	smsw	%ax
	testb	$0x4, %al	// EM bit in MSW
	setz	_fpu_present	// True if not set

	call	main	// main(argc, argv, envp)

	push	%eax	// push exit status
	call	exit

	hlt		// force a trap if exit fails

.section .data
.globl _penviron
_penviron:
	.long  _penvp                 // Pointer to environ, or hidden pointer

.section .bss
	.lcomm _penvp, 4             // Hidden environment vector
	.lcomm _fpu_present, 4       // FPU present flag
