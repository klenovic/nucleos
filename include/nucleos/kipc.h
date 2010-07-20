/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_KIPC_H
#define __NUCLEOS_KIPC_H

/* System call numbers that are passed when trapping to the kernel. */
#define KIPC_SEND	1	/* blocking send */
#define KIPC_RECEIVE	2	/* blocking receive */
#define KIPC_SENDREC	3	/* KIPC_SEND + KIPC_RECEIVE */
#define KIPC_NOTIFY	4	/* asynchronous notify */
#define KIPC_SENDNB	5	/* nonblocking send */
#define KIPC_SENDA	16	/* asynchronous send */

/* Defines for flags field */
#define AMF_EMPTY	0	/* slot is not inuse */
#define AMF_VALID	1	/* slot contains message */
#define AMF_DONE	2	/* Kernel has processed the message. The
				 * result is stored in 'result'
				 */
#define AMF_NOTIFY	4	/* Send a notification when AMF_DONE is set */
#define AMF_NOREPLY	8	/* Not a reply message for a KIPC_SENDREC */

#if defined(__KERNEL__) || defined(__UKERNEL__)
#ifndef __ASSEMBLY__

#include <nucleos/types.h>
#include <nucleos/type.h>

/* Masks and flags for system calls. */
#define NON_BLOCKING	0x0080  /* do not block if target not ready */

#define WILLRECEIVE(target, source_ep) \
	((RTS_ISSET(target, RTS_RECEIVING) && !RTS_ISSET(target, RTS_SENDING)) && \
		(target->p_getfrom_e == ENDPT_ANY || target->p_getfrom_e == source_ep))

typedef struct kipc_msg {
	endpoint_t m_source;	/* who sent the message */
	int m_type;		/* what kind of message is it */
	__s32 m_data1;
	__s32 m_data2;
	__s32 m_data3;
	__s32 m_data4;
	__s32 m_data5;
	__s32 m_data6;
	__s32 m_data7;
	__s32 m_data8;
	__s32 m_data9;
} kipc_msg_t;

/*==========================================================================* 
 * Nucleos run-time system (IPC). 					    *
 *==========================================================================*/ 

/* Datastructure for asynchronous sends */
typedef struct asynmsg {
	unsigned flags;
	endpoint_t dst;
	int result;
	kipc_msg_t msg;
} asynmsg_t;

#include <asm/kipc.h>

/* Kernel ipc routines. The __kipc_* are the arch-dependent implementations.
   Make them as fast as possible.
 */
static inline int kipc_notify(endpoint_t dst)
{
	return __kipc_notify(dst);
}

static inline int kipc_receive(endpoint_t src, kipc_msg_t *m_ptr)
{
	return __kipc_receive(src, m_ptr);
}

static inline int kipc_send(endpoint_t dst, kipc_msg_t *m_ptr, u32 flags)
{
	return __kipc_send(dst, m_ptr, flags);
}

static inline int kipc_senda(asynmsg_t *table, size_t count)
{
	return __kipc_senda(table, count);
}

static inline int kipc_sendnb(endpoint_t dst, kipc_msg_t *m_ptr)
{
	return __kipc_sendnb(dst, m_ptr);
}

static inline int kipc_sendrec(endpoint_t src_dst, kipc_msg_t *m_ptr, u32 flags)
{
	return __kipc_sendrec(src_dst, m_ptr, flags);
}

#endif /* __ASSEMBLY__ */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_KIPC_H */
