/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <nucleos/time.h>
#include <asm/syscall.h>

int gettimeofday(struct timeval *tv, void *tz)
{
	/* @nucleos: The  use  of  the timezone structure is obsolete;
	 *           the tz argument should normally be specified as NULL.
	 *           It has not been and will not be supported by libc or
	 *           glibc. Each and every occurrence of this field in the
	 *           kernel source (other than the declaration) is a bug.
	 */
	return INLINE_SYSCALL(gettimeofday, 2, tv, tz);
}
