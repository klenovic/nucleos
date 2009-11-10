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
#include <nucleos/com.h>
#include <nucleos/ioctl.h>

int ioctl(int fd, int request, void *data)
{
	message m;

	m.TTY_LINE = fd;
	m.TTY_REQUEST = request;
	m.ADDRESS = (char *) data;
	return(ksyscall(FS_PROC_NR, __NR_ioctl, &m));
}
