/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	svrctl() - special server control functions.	Author: Kees J. Bot
 *								24 Apr 1994
 */
#include <nucleos/lib.h>
#include <stdio.h>
#include <nucleos/svrctl.h>

int svrctl(int request, void *argp)
{
	message m;

	m.m2_i1 = request;
	m.m2_p1 = argp;

	switch ((request >> 8) & 0xFF) {
	case 'M':
	case 'S':
		/* PM handles calls for itself and the kernel. */
		return ksyscall(PM_PROC_NR, __NR_svrctl, &m);
	case 'F':
	case 'I':
		/* FS handles calls for itself and inet. */
		return ksyscall(FS_PROC_NR, __NR_svrctl, &m);
	default:
		errno = EINVAL;
		return -1;
	}
}
