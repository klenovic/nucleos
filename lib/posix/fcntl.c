/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <asm/syscall.h>
#include <stdarg.h>

int fcntl(int fd, int cmd, ...)
{
	va_list ap;
	void *arg;

	va_start (ap, cmd);
	arg = va_arg (ap, void *);
	va_end (ap);

	return INLINE_SYSCALL (fcntl, 3, fd, cmd, arg);
}
