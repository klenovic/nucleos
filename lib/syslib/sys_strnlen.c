/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>
#include <asm/servers/vm/memory.h>

/**
 * Get the size of a string specified by linear address.
 * @endpt  destination process endpoint
 * @s  The string to measure (linear address).
 * @maxlen  The maximum valid length
 * @return size of a NUL-terminated string.
 *
 * Returns the size of the string _including_ the terminating NULL.
 * On kernel exception, returns 0.
 * If the string is too long, returns a value greater than @n.
 */
long sys_strnlen(endpoint_t endpt, char __user *s, int maxlen)
{
	kipc_msg_t mess;

	mess.STRNLEN_PROC_E = endpt;
	mess.STRNLEN_STR = s;
	mess.STRNLEN_MAXLEN = maxlen;

	return(ktaskcall(SYSTASK, SYS_STRNLEN, &mess));
}
