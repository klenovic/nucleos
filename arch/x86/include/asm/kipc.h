/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_KIPC_H
#define __ASM_X86_KIPC_H

#include <nucleos/types.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/stringify.h>
#include <nucleos/errno.h>
#include <asm/irq_vectors.h>

/* Internal communication */
#define ASM_CALL_KIPC_SERVICE	"int $" __stringify(IRQVEC_KIPC) "\t\n"

/* New internal communication */
#define ASM_CALL_NKIPC_SERVICE	"int $" __stringify(IRQVEC_NKIPC) "\t\n"

/* the x86 uses registers for communication */
typedef struct __kipc_msg {
	__u32 ax;
	__u32 bx;
	__u32 cx;
	__u32 dx;
	__u32 di;
	__u32 si;
} __kipc_msg_t;

static inline int __kipc_notify(endpoint_t dst)
{
	int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(dst), "c"(KIPC_NOTIFY)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_receive(endpoint_t src, kipc_msg_t *msg)
{
	int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(src), "b"(msg), "c"(KIPC_RECEIVE)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_send(endpoint_t dst, kipc_msg_t *msg, u32 flags)
{
	int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(dst), "b"(msg), "c"(KIPC_SEND), "d"(flags)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_senda(asynmsg_t *table, size_t count)
{
	int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(count), "b"(table), "c"(KIPC_SENDA)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_sendrec(endpoint_t src_dst, kipc_msg_t *msg, u32 flags)
{
	int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(src_dst), "b"(msg), "c"(KIPC_SENDREC), "d"(flags)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_module_call(u8 type, u32 flags, endpoint_t endpt, void *msg)
{
	int ret = 0;

	if (type <= 0 || type > KIPC_SERVICES_COUNT)
		return -EPERM;

	__asm__ __volatile__ (
		ASM_CALL_NKIPC_SERVICE
		:"=a"(ret)
		:"0"(type), "b"(flags), "c"(endpt), "d"(msg)
		:"memory", "cc"
	);

	return ret;
}

#endif /* __ASM_X86_KIPC_H */
