/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_KIPC_H
#define __NUCLEOS_KIPC_H

/* This header file defines constants for nucleos inter-process communication.
 * These definitions are used in the file proc.c.
 */
#include <nucleos/types.h>
#include <nucleos/type.h>
#include <nucleos/kipc_defs.h>

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
	__s8 m_data10[KIPC_FLG_M3_STRLEN];
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

static inline int kipc_send(endpoint_t dst, kipc_msg_t *m_ptr)
{
	return __kipc_send(dst, m_ptr);
}

static inline int kipc_senda(asynmsg_t *table, size_t count)
{
	return __kipc_senda(table, count);
}

static inline int kipc_sendnb(endpoint_t dst, kipc_msg_t *m_ptr)
{
	return __kipc_sendnb(dst, m_ptr);
}

static inline int kipc_sendrec(endpoint_t src_dst, kipc_msg_t *m_ptr)
{
	return __kipc_sendrec(src_dst, m_ptr);
}

#endif /* __NUCLEOS_KIPC_H */
