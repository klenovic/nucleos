/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* General macros and constants used by the kernel. */
#ifndef __KERNEL_CONST_H
#define __KERNEL_CONST_H

#include <nucleos/bitmap.h>
#include <asm/kernel/const.h>

#if defined (__KERNEL__) || defined (__UKERNEL__)

/* Some constant macros are used in both assembler and
 * C code.  Therefore we cannot annotate them always with
 * 'UL' and other type specifiers unilaterally.  We
 * use the following macros to deal with this.
 *
 * Similarly, _AT() will cast an expression with a type in C, but
 * leave it unchanged in asm.
 */
#ifdef __ASSEMBLY__
#define _AC(X,Y)  X
#define _AT(T,X)  X
#else
#define __AC(X,Y)  (X##Y)
#define _AC(X,Y)   __AC(X,Y)
#define _AT(T,X)   ((T)(X))
#endif

/* Map a process number to a privilege structure id. */
#define s_nr_to_id(n)	(NR_TASKS + (n) + 1)

/* Translate a pointer to a field in a structure to a pointer to the structure
 * itself. So it translates '&struct_ptr->field' back to 'struct_ptr'.
 */
#define structof(type, field, ptr) \
	((type *) (((char *) (ptr)) - offsetof(type, field)))

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#define _SRC_	0
#define _DST_	1

#define get_sys_bit(map,bit) \
	( MAP_CHUNK(map.chunk,bit) & (1 << CHUNK_OFFSET(bit) )
#define get_sys_bits(map,bit) \
	( MAP_CHUNK(map.chunk,bit) )
#define set_sys_bit(map,bit) \
	( MAP_CHUNK(map.chunk,bit) |= (1 << CHUNK_OFFSET(bit) )
#define unset_sys_bit(map,bit) \
	( MAP_CHUNK(map.chunk,bit) &= ~(1 << CHUNK_OFFSET(bit) )
#define NR_SYS_CHUNKS	BITMAP_CHUNKS(NR_SYS_PROCS)
#endif /* !(__KERNEL__ || __UKERNEL__) */

#ifdef __KERNEL__
/* Translate an endpoint number to a process number, return success. */
#define isokendpt(e,p) isokendpt_d((e),(p),0)
#define okendpt(e,p)   isokendpt_d((e),(p),1)

#define reallock										\
	do {											\
		int d;										\
		d = intr_disabled();								\
		intr_disable();									\
		locklevel++;									\
		if(d && locklevel == 1) {							\
			minix_panic("reallock while interrupts disabled first time", __LINE__);	\
		} \
	} while(0)

#define realunlock									\
	do {										\
		if(!intr_disabled()) {							\
			minix_panic("realunlock while interrupts enabled", __LINE__);	\
		}									\
		if(locklevel < 1) {							\
			minix_panic("realunlock while locklevel below 1", __LINE__);	\
		} locklevel--;								\
		if(locklevel == 0) {							\
			intr_enable();							\
		}									\
	} while(0)

/* Disable/ enable hardware interrupts. The parameters of lock() and unlock()
 * are used when debugging is enabled.
 */
#define lock      reallock
#define unlock    realunlock

/* args to intr_init() */
#define INTS_ORIG	0	/* restore interrupts */
#define INTS_MINIX	1	/* initialize interrupts for minix */

/* for kputc() */
#define END_OF_KMESS	0

#endif /* !__KERNEL__ */

#if defined (__KERNEL__) || defined (__UKERNEL__)
/* This section contains defines for valuable system resources that are used
 * by device drivers. The number of elements of the vectors is determined by
 * the maximum needed by any given driver. The number of interrupt hooks may
 * be incremented on systems with many device drivers.
 */
#define K_STACK_BYTES	CONFIG_KERNEL_STACK_SIZE
#define VDEVIO_BUF_SIZE	CONFIG_KERNEL_VDEVIO_BUF_SIZE
#define VCOPY_VEC_SIZE	CONFIG_KERNEL_VCOPY_VEC_SIZE
#define NR_IRQ_HOOKS	CONFIG_KERNEL_NR_IRQ_HOOKS
#define KMESS_BUF_SIZE	CONFIG_KERNEL_MESSAGES_BUFFER_SIZE

/* Length of program names stored in the process table. This is only used
 * for the debugging dumps that can be generated with the IS server. The PM
 * server keeps its own copy of the program name.
 */
#define P_NAME_LEN	CONFIG_KERNEL_P_NAME_LEN

/* In embedded and sensor applications, not all the kernel calls may be
 * needed. In this section you can specify which kernel calls are needed
 * and which are not. The code for unneeded kernel calls is not included in
 * the system binary, making it smaller. If you are not sure, it is best
 * to keep all kernel calls enabled.
 */
#define USE_FORK		1	/* fork a new process */
#define USE_NEWMAP		1	/* set a new memory map */
#define USE_EXEC		1	/* update process after execute */
#define USE_EXIT		1	/* clean up after process exit */
#define USE_TRACE		1	/* process information and tracing */
#define USE_GETKSIG		1	/* retrieve pending kernel signals */
#define USE_ENDKSIG		1	/* finish pending kernel signals */
#define USE_KILL		1 	/* send a signal to a process */
#define USE_SIGSEND		1	/* send POSIX-style signal */
#define USE_SIGRETURN		1	/* sys_sigreturn(proc_nr, ctxt_ptr, flags) */
#define USE_ABORT		1	/* shut down MINIX */
#define USE_GETINFO		1 	/* retrieve a copy of kernel data */
#define USE_TIMES		1	/* get process and system time info */
#define USE_SETALARM		1	/* schedule a synchronous alarm */
#define USE_VTIMER		1	/* set or retrieve a process-virtual timer */
#define USE_DEVIO		1	/* read or write a single I/O port */
#define USE_VDEVIO		1	/* process vector with I/O requests */
#define USE_SDEVIO		1	/* perform I/O request on a buffer */
#define USE_IRQCTL		1	/* set an interrupt policy */
#define USE_SEGCT		1	/* set up a remote segment */
#define USE_PRIVCTL		1	/* system privileges control */
#define USE_NICE		1	/* change scheduling priority */
#define USE_UMAP		1	/* map virtual to physical address */
#define USE_VIRCOPY		1	/* copy using virtual addressing */ 
#define USE_VIRVCOPY		1	/* vector with virtual copy requests */
#define USE_PHYSCOPY		1 	/* copy using physical addressing */
#define USE_PHYSVCOPY		1	/* vector with physical copy requests */
#define USE_MEMSET		1	/* write char to a given memory area */
#define USE_RUNCTL		1	/* control stop flags of process */
#endif /* !(__KERNEL__ || __UKERNEL__) */

#endif /* __KERNEL_CONST_H */
