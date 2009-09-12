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

extern char *_brksize;

/* Both OSF/1 and SYSVR4 man pages specify that brk(2) returns int.
 * However, BSD4.3 specifies that brk() returns char*.  POSIX omits
 * brk() on the grounds that it imposes a memory model on an architecture.
 * For this reason, brk() and sbrk() are not in the lib/posix directory.
 * On the other hand, they are so crucial to correct operation of so many
 * parts of the system, that we have chosen to hide the name brk using brk,
 * as with system calls.  In this way, if a user inadvertently defines a
 * procedure brk, MINIX may continue to work because the true call is brk.
 */
int brk(char *addr)
{
	message m;

	if (addr != _brksize) {
		m.m1_p1 = addr;

		if (_syscall(PM_PROC_NR, __NR_brk, &m) < 0)
			return(-1);

		_brksize = m.m2_p1;
	}

	return(0);
}

