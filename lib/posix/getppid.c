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

pid_t getppid()
{
	message m;

	/* POSIX says that this function is always successful and that no
	 * return value is reserved to indicate an error.  Minix syscalls
	 * are not always successful and Minix returns the reserved value
	 * (pid_t) -1 when there is an error.
	 */
	if (ksyscall(PM_PROC_NR, __NR_getpid, &m) < 0)
		return ( (pid_t) -1);

	return( (pid_t) m.m2_i1);
}
