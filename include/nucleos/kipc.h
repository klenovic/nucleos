/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _NUCLEOS_KIPC_H
#define _NUCLEOS_KIPC_H

/* System call numbers that are passed when trapping to the kernel. */
#define KIPC_SEND	1	/* blocking send */
#define KIPC_RECEIVE	2	/* blocking receive */
#define KIPC_SENDREC	3	/* KIPC_SEND + KIPC_RECEIVE */
#define KIPC_NOTIFY	4	/* asynchronous notify */
#define KIPC_SENDA	5	/* asynchronous send */

#define KIPC_SERVICES_COUNT	5	/* number of kipc services
					 * @nucleos: Temporary until the services
					 *           are merged.
					 */

/* Masks and flags for system calls. */
#define KIPC_FLG_NONBLOCK	1  /* do not block if target not ready */

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

#define WILLRECEIVE(target, source_ep) \
	((RTS_ISSET(target, RTS_RECEIVING) && !RTS_ISSET(target, RTS_SENDING)) && \
		(target->p_getfrom_e == ENDPT_ANY || target->p_getfrom_e == source_ep))

/**
 * Generic message type common for all architecture. It is up to architecture to
 * make the conversions arch<->generic i.e. must implement the appropriate convertors.
 */
typedef struct kipc_msg {
	endpoint_t m_source;	/* who sent the message (filled in by kernel) */
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

/**
 * Interface for kernel internal communication used by modules.
 */
static inline int kipc_module_call(u8 type, u32 flags, endpoint_t endpt, void *msg)
{
	return __kipc_module_call(type, flags, endpt, msg);
}

static inline int ktaskcall(endpoint_t who, int syscallnr, register kipc_msg_t *msgptr)
{
	int status;

	msgptr->m_type = syscallnr;
	status = kipc_module_call(KIPC_SENDREC, 0, who, msgptr);

	if (status != 0)
		return(status);

	return(msgptr->m_type);
}

#endif /* __ASSEMBLY__ */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* _NUCLEOS_KIPC_H */
