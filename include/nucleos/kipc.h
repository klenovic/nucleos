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

/*==========================================================================* 
 * Types relating to messages. 						    *
 *==========================================================================*/ 

typedef struct {
	int m1i1;
	int m1i2;
	int m1i3;
	char *m1p1;
	char *m1p2;
	char *m1p3;
} mess_1;

typedef struct {
	int m2i1;
	int m2i2;
	int m2i3;
	long m2l1;
	long m2l2;
	char *m2p1;
	short m2s1;
} mess_2;

typedef struct {
	int m3i1;
	int m3i2;
	char *m3p1;
	char m3ca1[KIPC_FLG_M3_STRLEN];
} mess_3;

typedef struct {
	long m4l1;
	long m4l2;
	long m4l3;
	long m4l4;
	long m4l5;
} mess_4;

typedef struct {
	short m5c1;
	short m5c2;
	int m5i1;
	int m5i2;
	long m5l1;
	long m5l2;
	long m5l3;
} mess_5;

typedef struct {
	long m6l1;
	long m6l2;
	long m6l3;
	short m6s1;
	short m6s2;
	short m6s3;
	char m6c1;
	char m6c2;
	char *m6p1;
	char *m6p2;
} mess_6;

typedef struct {
	int m7i1;
	int m7i2;
	int m7i3;
	int m7i4;
	char *m7p1;
	char *m7p2;
} mess_7;

typedef struct {
	int m8i1;
	int m8i2;
	char *m8p1;
	char *m8p2;
	char *m8p3;
	char *m8p4;
} mess_8;

typedef struct {
	long m9l1;
	long m9l2;
	long m9l3;
	long m9l4;
	long m9l5;
	short m9s1;
	short m9s2;
	short m9s3;
	char m9c1;
	char m9c2;
} mess_9;

typedef struct {
	endpoint_t m_source;	/* who sent the message */
	int m_type;		/* what kind of message is it */
	union {
		mess_1 m_m1;
		mess_2 m_m2;
		mess_3 m_m3;
		mess_4 m_m4;
		mess_5 m_m5;
		mess_7 m_m7;
		mess_8 m_m8;
		mess_6 m_m6;
		mess_9 m_m9;
	} m_u;
} message;

/* The following defines provide names for useful members. */
#define m1_i1  m_u.m_m1.m1i1
#define m1_i2  m_u.m_m1.m1i2
#define m1_i3  m_u.m_m1.m1i3
#define m1_p1  m_u.m_m1.m1p1
#define m1_p2  m_u.m_m1.m1p2
#define m1_p3  m_u.m_m1.m1p3

#define m2_i1  m_u.m_m2.m2i1
#define m2_i2  m_u.m_m2.m2i2
#define m2_i3  m_u.m_m2.m2i3
#define m2_l1  m_u.m_m2.m2l1
#define m2_l2  m_u.m_m2.m2l2
#define m2_p1  m_u.m_m2.m2p1

#define m2_s1  m_u.m_m2.m2s1

#define m3_i1  m_u.m_m3.m3i1
#define m3_i2  m_u.m_m3.m3i2
#define m3_p1  m_u.m_m3.m3p1
#define m3_ca1 m_u.m_m3.m3ca1

#define m4_l1  m_u.m_m4.m4l1
#define m4_l2  m_u.m_m4.m4l2
#define m4_l3  m_u.m_m4.m4l3
#define m4_l4  m_u.m_m4.m4l4
#define m4_l5  m_u.m_m4.m4l5

#define m5_c1  m_u.m_m5.m5c1
#define m5_c2  m_u.m_m5.m5c2
#define m5_i1  m_u.m_m5.m5i1
#define m5_i2  m_u.m_m5.m5i2
#define m5_l1  m_u.m_m5.m5l1
#define m5_l2  m_u.m_m5.m5l2
#define m5_l3  m_u.m_m5.m5l3

#define m6_l1  m_u.m_m6.m6l1
#define m6_l2  m_u.m_m6.m6l2
#define m6_l3  m_u.m_m6.m6l3
#define m6_s1  m_u.m_m6.m6s1
#define m6_s2  m_u.m_m6.m6s2
#define m6_s3  m_u.m_m6.m6s3
#define m6_c1  m_u.m_m6.m6c1
#define m6_c2  m_u.m_m6.m6c2
#define m6_p1  m_u.m_m6.m6p1
#define m6_p2  m_u.m_m6.m6p2

#define m7_i1  m_u.m_m7.m7i1
#define m7_i2  m_u.m_m7.m7i2
#define m7_i3  m_u.m_m7.m7i3
#define m7_i4  m_u.m_m7.m7i4
#define m7_p1  m_u.m_m7.m7p1
#define m7_p2  m_u.m_m7.m7p2

#define m8_i1  m_u.m_m8.m8i1
#define m8_i2  m_u.m_m8.m8i2
#define m8_p1  m_u.m_m8.m8p1
#define m8_p2  m_u.m_m8.m8p2
#define m8_p3  m_u.m_m8.m8p3
#define m8_p4  m_u.m_m8.m8p4

#define m9_l1  m_u.m_m9.m9l1
#define m9_l2  m_u.m_m9.m9l2
#define m9_l3  m_u.m_m9.m9l3
#define m9_l4  m_u.m_m9.m9l4
#define m9_l5  m_u.m_m9.m9l5
#define m9_s1  m_u.m_m9.m9s1
#define m9_s2  m_u.m_m9.m9s2
#define m9_s3  m_u.m_m9.m9s3
#define m9_c1  m_u.m_m9.m9c1
#define m9_c2  m_u.m_m9.m9c2

/*==========================================================================* 
 * Nucleos run-time system (IPC). 					    *
 *==========================================================================*/ 

/* Datastructure for asynchronous sends */
typedef struct asynmsg {
	unsigned flags;
	endpoint_t dst;
	int result;
	message msg;
} asynmsg_t;

#include <asm/kipc.h>

/* Kernel ipc routines. The __kipc_* are the arch-dependent implementations.
   Make them as fast as possible.
 */
static inline int kipc_notify(endpoint_t dst)
{
	return __kipc_notify(dst);
}

static inline int kipc_receive(endpoint_t src, message *m_ptr)
{
	return __kipc_receive(src, m_ptr);
}

static inline int kipc_send(endpoint_t dst, message *m_ptr)
{
	return __kipc_send(dst, m_ptr);
}

static inline int kipc_senda(asynmsg_t *table, size_t count)
{
	return __kipc_senda(table, count);
}

static inline int kipc_sendnb(endpoint_t dst, message *m_ptr)
{
	return __kipc_sendnb(dst, m_ptr);
}

static inline int kipc_sendrec(endpoint_t src_dst, message *m_ptr)
{
	return __kipc_sendrec(src_dst, m_ptr);
}

#endif /* __NUCLEOS_KIPC_H */
