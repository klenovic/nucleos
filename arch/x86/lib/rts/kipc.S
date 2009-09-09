/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* See include/kernel/kipc.h for C definitions */
#include <kernel/kipc.h>

//========================================================================
//                           IPC assembly routines
//========================================================================
// all message passing routines save ebp, but destroy eax and ecx.
.section .text
.globl kipc_send
kipc_send:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	mov	KIPC_SRCDST(%ebp), %eax      // eax = dest-src
	mov	KIPC_MESSAGE(%ebp), %ebx      // ebx = message pointer
	mov	$KIPC_SEND,%ecx               // kipc_send(dest, ptr)
	int	$KERN_SYSVEC                  // trap to the kernel
	pop	%ebx
	pop	%ebp
	ret

.globl kipc_receive
kipc_receive:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	mov	KIPC_SRCDST(%ebp), %eax     // eax = dest-src
	mov	KIPC_MESSAGE(%ebp), %ebx     // ebx = message pointer
	mov	$KIPC_RECEIVE, %ecx          // kipc_receive(src, ptr)
	int	$KERN_SYSVEC                 // trap to the kernel
	pop	%ebx
	pop	%ebp
	ret

.globl kipc_sendrec
kipc_sendrec:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	mov	KIPC_SRCDST(%ebp), %eax      // eax = dest-src
	mov	KIPC_MESSAGE(%ebp), %ebx      // ebx = message pointer
	mov	$KIPC_SENDREC, %ecx           // kipc_sendrec(srcdest, ptr)
	int	$KERN_SYSVEC                  // trap to the kernel
	pop	%ebx
	pop	%ebp
	ret

.globl kipc_notify
kipc_notify:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	mov	KIPC_SRCDST(%ebp), %eax      // eax = destination 
	mov	$KIPC_NOTIFY, %ecx            // kipc_notify(srcdst)
	int	$KERN_SYSVEC                  // trap to the kernel
	pop	%ebx
	pop	%ebp
	ret

.globl kipc_sendnb
kipc_sendnb:
	push	%ebp
	mov	%esp, %ebp
	push	%ebx
	mov	KIPC_SRCDST(%ebp),%eax        // eax = dest-src
	mov	KIPC_MESSAGE(%ebp),%ebx        // ebx = message pointer
	mov	$KIPC_SENDNB, %ecx             // kipc_sendnb(dest, ptr)
	int	$KERN_SYSVEC                   // trap to the kernel
	pop	%ebx
	pop	%ebp
	ret

.globl kipc_senda
kipc_senda:
	push	%ebp
	mov	%esp,%ebp
	push	%ebx
	mov	KIPC_TABCOUNT(%ebp), %eax 	// eax = count
	mov	KIPC_MSGTAB(%ebp), %ebx  	// ebx = table
	mov	$KIPC_SENDA, %ecx  		// kipc_senda(table, count)
	int	$KERN_SYSVEC			// trap to the kernel
	pop	%ebx
	pop	%ebp
	ret
