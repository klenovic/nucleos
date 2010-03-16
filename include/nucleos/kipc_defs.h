/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_KIPC_DEFS_H
#define __NUCLEOS_KIPC_DEFS_H

/* This header file defines constants for nucleos inter-process communication.
 * Keep in mind that it can be included assembly files too.
 */

#include <asm/kipc_defs.h>

/* System call numbers that are passed when trapping to the kernel. */
#define KIPC_SEND	1	/* blocking send */
#define KIPC_RECEIVE	2	/* blocking receive */
#define KIPC_SENDREC	3	/* KIPC_SEND + KIPC_RECEIVE */
#define KIPC_NOTIFY	4	/* asynchronous notify */
#define KIPC_SENDNB	5	/* nonblocking send */
#define KIPC_SENDA	16	/* asynchronous send */

/*==========================================================================* 
 * Types relating to messages. 						    *
 *==========================================================================*/ 

/* message flags */
#define KIPC_FLG_M3_STRLEN	14	/* string length */

/* Defines for flags field */
#define AMF_EMPTY	0	/* slot is not inuse */
#define AMF_VALID	1	/* slot contains message */
#define AMF_DONE	2	/* Kernel has processed the message. The
				 * result is stored in 'result'
				 */
#define AMF_NOTIFY	4	/* Send a notification when AMF_DONE is set */
#define AMF_NOREPLY	8	/* Not a reply message for a KIPC_SENDREC */

#ifdef __KERNEL__
#ifndef __ASSEMBLY__

/* Masks and flags for system calls. */
#define NON_BLOCKING    0x0080  /* do not block if target not ready */

#define WILLRECEIVE(target, source_ep) \
	((RTS_ISSET(target, RTS_RECEIVING) && !RTS_ISSET(target, RTS_SENDING)) && \
		(target->p_getfrom_e == ENDPT_ANY || target->p_getfrom_e == source_ep))

#endif /* __ASSEMBLY__ */
#endif /* __KERNEL__ */

#endif /* __NUCLEOS_KIPC_DEFS_H */
