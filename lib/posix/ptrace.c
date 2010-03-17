/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

/* long ptrace(int request, pid_t pid, long *addr, long *data) */
long ptrace(int request, ...)
{
	long int res, ret;
	va_list ap;
	pid_t pid;
	void *addr, *data;

	va_start (ap, request);
	pid = va_arg (ap, pid_t);
	addr = va_arg (ap, void *);
	data = va_arg (ap, void *);
	va_end (ap);

	if (request > 0 && request < 4)
		data = &ret;

	res = INLINE_SYSCALL (ptrace, 4, request, pid, addr, data);

	if (res >= 0 && request > 0 && request < 4) {
		__set_errno (0);
		return ret;
	}

	return res;
}
