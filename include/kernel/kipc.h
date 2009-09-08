/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __KERNEL_KIPC_H
#define __KERNEL_KIPC_H

/* This header file defines constants for Nucleos inter-process communication.
 * These definitions are used in the file proc.c.
 */
#include <nucleos/com.h>

/* System call numbers that are passed when trapping to the kernel. */
#define KIPC_SEND	1	/* blocking send */
#define KIPC_RECEIVE	2	/* blocking receive */
#define KIPC_SENDREC	3	/* KIPC_SEND + KIPC_RECEIVE */
#define KIPC_NOTIFY	4	/* asynchronous notify */
#define KIPC_SENDNB	5	/* nonblocking send */
#define KIPC_SENDA	16	/* asynchronous send */
#define KIPC_SRCDST	8	/* source/destination process */
#define KIPC_MESSAGE	12	/* message pointer */
#define KIPC_MSGTAB	8	/* message table */
#define KIPC_TABCOUNT	12	/* number of entries in message table */

#define KERN_SYSVEC	33	/* trap to kernel */

#ifndef __ASSEMBLY__

/* Masks and flags for system calls. */
#define NON_BLOCKING	0x0080  /* do not block if target not ready */

/* The following bit masks determine what checks that should be done. */
#define CHECK_DEADLOCK  0x03	/* 0000 0011 : check for deadlock */

#define WILLRECEIVE(target, source_ep) \
  ((RTS_ISSET(target, RECEIVING) && !RTS_ISSET(target, SENDING)) &&	\
    (target->p_getfrom_e == ANY || target->p_getfrom_e == source_ep))

#endif /* __ASSEMBLY__ */

#endif /* __KERNEL_KIPC_H */
