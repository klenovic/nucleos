/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _CONFIG_H
#define _CONFIG_H

/* This file sets configuration parameters for the MINIX kernel, FS, and PM.
 * It is divided up into two main sections.  The first section contains
 * user-settable parameters.  In the second section, various internal system
 * parameters are set based on the user-settable parameters.
 */

/*===========================================================================*
 *              This section contains user-settable parameters               *
 *===========================================================================*/
/* Word size in bytes (a constant equal to sizeof(int)). */
#ifdef __GNUC__
#define _WORD_SIZE      _EM_WSIZE
#define _PTR_SIZE       _EM_WSIZE
#endif

#define _NR_PROCS       100
#define _NR_SYS_PROCS   32

/* Number of slots in the process table for non-kernel processes. The number
 * of system processes defines how many processes with special privileges 
 * there can be. User processes share the same properties and count for one. 
 */
#define NR_PROCS 	  _NR_PROCS 
#define NR_SYS_PROCS      _NR_SYS_PROCS

/* Number of controller tasks (/dev/cN device classes). */
#define NR_CTRLRS          2

/* DMA_SECTORS may be increased to speed up DMA based drivers. */
#define DMA_SECTORS        1	/* DMA buffer size (must be >= 1) */

/* Which processes should receive diagnostics from the kernel and system? 
 * Directly sending it to TTY only displays the output. Sending it to the
 * log driver will cause the diagnostics to be buffered and displayed.
 * Messages are sent by src/lib/sysutil/kputc.c to these processes, in
 * the order of this array, which must be terminated by NONE. This is used
 * by drivers and servers that printf().
 * The kernel does this for its own kprintf() in kernel/utility.c, also using
 * this array, but a slightly different mechanism.
 */
#define OUTPUT_PROCS_ARRAY	{ TTY_PROC_NR, LOG_PROC_NR, NONE }

/* NR_CONS, NR_RS_LINES, and NR_PTYS determine the number of terminals the
 * system can handle.
 */
#define NR_CONS            4	/* # system consoles (1 to 8) */
#define	NR_RS_LINES	   4	/* # rs232 terminals (0 to 4) */
#define	NR_PTYS		   32	/* # pseudo terminals (0 to 64) */

/* This feature enable the counting of system calls in PM and FS */
#define ENABLE_SYSCALL_STATS	0

/*===========================================================================*
 *	There are no user-settable parameters after this line		     *
 *===========================================================================*/
#define ASKDEV _ASKDEV
#define FASTLOAD _FASTLOAD

/* Enable or disable system profiling. */
#define SPROFILE          1    /* statistical profiling */
#define CPROFILE          0    /* call profiling */

/* Compile kernel so that first page of code and data can be unmapped. */
#define VM_KERN_NOPAGEZERO      1

#endif /* _CONFIG_H */
