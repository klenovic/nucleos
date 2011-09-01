/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <stdarg.h>
#include <nucleos/syslib.h>
#include <nucleos/unistd.h>

int sys_abort(int how)
{
/* Something awful has happened. Abandon ship. */
	kipc_msg_t m;
	m.ABRT_HOW = how;
	return(ktaskcall(SYSTASK, SYS_ABORT, &m));
}
