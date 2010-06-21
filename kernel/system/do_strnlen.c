/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:	SYS_STRNLEN
 *
 * The parameters for this kernel call are:
 *     m_data1: STRNLEN_PROC_E	process endpoint
 *     m_data4: STRNLEN_STR	string (user)
 *     m_data2: STRNLEN_MAXLEN	max. length
 */
#include <nucleos/type.h>
#include <nucleos/const.h>
#include <nucleos/kipc.h>
#include <kernel/proc.h>
#include <kernel/system.h>
#include <asm/kernel/uaccess.h>

/**
 * Get the size of a string specified by linear address.
 * @proc  process address
 * @s  The string to measure (linear address).
 * @maxlen  The maximum valid length
 *
 * Get the size of a NUL-terminated string.
 *
 * Returns the size of the string _including_ the terminating NULL.
 * On kernel exception, returns 0.
 * If the string is too long, returns a value greater than @maxlen.
 */
long do_strnlen(kipc_msg_t *m_ptr)
{
	int proc_nr;
	struct proc *p;

	if (!m_ptr->STRNLEN_STR || !isokendpt(m_ptr->STRNLEN_PROC_E, &proc_nr) ||
	    m_ptr->STRNLEN_MAXLEN <= 0)
		return -EINVAL;

	p = proc_addr(proc_nr);

	return strnlen_user(p, m_ptr->STRNLEN_STR, m_ptr->STRNLEN_MAXLEN);
}
