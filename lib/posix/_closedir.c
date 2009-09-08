/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	closedir()					Author: Kees J. Bot
 *								24 Apr 1989
 */
#include <nucleos/lib.h>
#include <nucleos/types.h>
#include <nucleos/dirent.h>
#include <nucleos/unistd.h>
#include <stdlib.h>
#include <nucleos/errno.h>

int closedir(DIR *dp)
/* Finish reading a directory. */
{
	int d;

	if (dp == 0) { errno= EBADF; return -1; }

	d= dp->_fd;
	free((void *) dp);
	return close(d);
}
