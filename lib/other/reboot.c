/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* reboot.c - Systemcall interface to mm/signal.c::do_reboot()

   author: Edvard Tuinder  v892231@si.hhs.NL
 */

#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int reboot(int how, ...)
{
	void *code = 0;
	size_t size = 0;
	va_list ap;

	va_start(ap, how);
	if (how == RBT_MONITOR) {
		code = va_arg(ap, void*);
		size = va_arg(ap, size_t);
	}
	va_end(ap);

	return INLINE_SYSCALL(reboot, 3, how, code, size);
}
