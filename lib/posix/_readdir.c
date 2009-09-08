/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	readdir()					Author: Kees J. Bot
 *								24 Apr 1989
 */
#include <nucleos/types.h>
#include <nucleos/stat.h>
#include <nucleos/dirent.h>
#include <nucleos/unistd.h>
#include <nucleos/stddef.h>
#include <stdlib.h>
#include <nucleos/fcntl.h>
#include <nucleos/limits.h>
#include <nucleos/errno.h>
#include <nucleos/string.h>

struct dirent *readdir(DIR *dp)
/* Return the next entry in a directory. */
{
	struct dirent *entp;
	int count, pos, reclen;

	if (dp == 0) { errno= EBADF; return 0; }

	count= dp->_count;
	pos= dp->_pos;
	if (count == 0 || pos >= count)
	{
		count= getdents(dp->_fd, (struct dirent *)dp->_buf,
			sizeof(dp->_buf));
		if (count <= 0) return 0;
		dp->_count= count;
		dp->_pos= pos= 0;
		}
	entp= (struct dirent *)&((char *)dp->_buf)[pos];
	reclen= entp->d_reclen;
	dp->_pos= pos+reclen;

	return entp;
}
