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
#define nil 0
#define readdir	_readdir
#define getdents _getdents
#include <nucleos/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <nucleos/stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <nucleos/limits.h>
#include <nucleos/errno.h>
#include <string.h>

struct dirent *readdir(DIR *dp)
/* Return the next entry in a directory. */
{
	struct dirent *entp;
	int count, pos, reclen;

	if (dp == nil) { errno= EBADF; return nil; }

	count= dp->_count;
	pos= dp->_pos;
	if (count == 0 || pos >= count)
	{
		count= getdents(dp->_fd, (struct dirent *)dp->_buf,
			sizeof(dp->_buf));
		if (count <= 0) return nil;
		dp->_count= count;
		dp->_pos= pos= 0;
		}
	entp= (struct dirent *)&((char *)dp->_buf)[pos];
	reclen= entp->d_reclen;
	dp->_pos= pos+reclen;

	return entp;
}
