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
	__s32 m1;
	__s32 m2;
	__s32 m3;
	__s32 m4;
	__s32 m5;
	__s32 m6;
	__s32 m7;
	__s32 m8;
	__s32 m9;
	__s8 m10[KIPC_FLG_M3_STRLEN];
} kipc_msg_t;

/* The following defines provide names for useful members. */
#define m1_i1  m1
#define m1_i2  m2
#define m1_i3  m3
#define m1_p1  m4
#define m1_p2  m5
#define m1_p3  m6

#define m2_i1  m1
#define m2_i2  m2
#define m2_i3  m3
#define m2_l1  m4
#define m2_l2  m5
#define m2_p1  m6
#define m2_s1  m7

#define m3_i1  m1
#define m3_i2  m2
#define m3_p1  m3
#define m3_ca1 m10

#define m4_l1  m1
#define m4_l2  m2
#define m4_l3  m3
#define m4_l4  m4
#define m4_l5  m5

#define m5_c1  m1
#define m5_c2  m2
#define m5_i1  m3
#define m5_i2  m4
#define m5_l1  m5
#define m5_l2  m6
#define m5_l3  m7

#define m7_i1  m1
#define m7_i2  m2
#define m7_i3  m3
#define m7_i4  m4
#define m7_p1  m5
#define m7_p2  m6

#define m8_i1  m1
#define m8_i2  m2
#define m8_p1  m3
#define m8_p2  m4
#define m8_p3  m5
#define m8_p4  m6

#define m9_l1  m1
#define m9_l2  m2
#define m9_l3  m3
#define m9_l4  m4
#define m9_l5  m5
#define m9_s1  m6
#define m9_s2  m7
#define m9_s3  m8
#define m9_s4  m9

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
