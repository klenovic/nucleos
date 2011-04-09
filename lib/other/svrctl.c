/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <stdio.h>
#include <nucleos/lib.h>
#include <nucleos/svrctl.h>

int svrctl(int request, void *argp)
{
	switch ((request >> 8) & 0xFF) {
	case 'M':
	case 'S':
		/* PM handles calls for itself and the kernel. */
		return INLINE_SYSCALL(svrctl, 3, PM_PROC_NR, request, argp);
	case 'F':
	case 'I':
		/* FS handles calls for itself and inet. */
		return INLINE_SYSCALL(svrctl, 3, VFS_PROC_NR, request, argp);
	default:
		errno = EINVAL;
		return -1;
	}

	return 0;
}
