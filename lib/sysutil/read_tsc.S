/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
.section .text

.globl	read_tsc	// read the cycle counter (Pentium and up)
//===========================================================================
// void read_tsc(unsigned long *high, unsigned long *low);
// Read the cycle counter of the CPU. Pentium and up. 
.p2align 4
read_tsc:
	push %edx
	push %eax
	rdtsc		// it places the TSC in EDX:EAX
	push %ebp
	mov 16(%esp), %ebp
	mov %edx, (%ebp)
	mov 20(%esp), %ebp
	mov %eax, (%ebp)
	pop %ebp
	pop %eax
	pop %edx
	ret

//===========================================================================*
// PUBLIC void read_host_time_ns(unsigned long *high, unsigned long *low);
// access real time in ns from host in vmware.
.p2align 4
read_host_time_ns:
	push %edx
	push %eax
	push %ecx
	mov $0x10001, %ecx
	rdpmc		// it places the result in EDX:EAX
	push %ebp
	mov 20(%esp), %ebp
	mov %edx, (%ebp)
	mov 24(%esp), %ebp
	mov %eax, (%ebp)
	pop %ebp
	pop %ecx
	pop %eax
	pop %edx
	ret
