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
#include <nucleos/stat.h>

int fstat(int fd, struct stat *buffer)
{
	message m;

	m.m1_i1 = fd;
	m.m1_p1 = (char *) buffer;

	return(_syscall(FS_PROC_NR, __NR_fstat, &m));
}
