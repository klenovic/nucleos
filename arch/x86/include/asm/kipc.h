/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <asm/irq_vectors.h>

/* Internal communication */
#define ASM_CALL_KIPC_SERVICE	"int $" __stringify(SYS386_VECTOR) "\t\n"

static inline int __kipc_notify(endpoint_t dst)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(dst), "c"(KIPC_NOTIFY)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_receive(endpoint_t src, message *m_ptr)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(src), "b"(m_ptr), "c"(KIPC_RECEIVE)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_send(endpoint_t dst, message *m_ptr)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(dst), "b"(m_ptr), "c"(KIPC_SEND)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_senda(asynmsg_t *table, size_t count)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(count), "b"(table), "c"(KIPC_SENDA)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_sendnb(endpoint_t dst, message *m_ptr)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(dst), "b"(m_ptr), "c"(KIPC_SENDNB)
		:"memory", "cc"
	);

	return ret;
}

static inline int __kipc_sendrec(endpoint_t src_dst, message *m_ptr)
{
	register int ret = 0;

	__asm__ __volatile__ (
		ASM_CALL_KIPC_SERVICE
		:"=a"(ret)
		:"0"(src_dst), "b"(m_ptr), "c"(KIPC_SENDREC)
		:"memory", "cc"
	);

	return ret;
}

#endif /* __ASM_X86_KIPC_H */
