/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	seekdir()					Author: Kees J. Bot
 *								24 Apr 1989
 */
#include <nucleos/lib.h>
#include <nucleos/types.h>
#include <nucleos/dirent.h>
#include <nucleos/unistd.h>
#include <nucleos/errno.h>

int seekdir(DIR *dp, off_t pos)
/* Seek to position pos in a directory. */
{
	int off;

	if (dp == 0) { errno= EBADF; return -1; }

	dp->_count= 0;

	if (lseek(dp->_fd, pos, SEEK_SET) == -1) return -1;

	return 0;
}
