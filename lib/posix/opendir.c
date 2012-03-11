/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	opendir()					Author: Kees J. Bot
 *								24 Apr 1989
 */
#include <nucleos/lib.h>
#include <nucleos/types.h>
#include <nucleos/stat.h>
#include <nucleos/dirent.h>
#include <nucleos/unistd.h>
#include <stdlib.h>
#include <nucleos/fcntl.h>
#include <nucleos/errno.h>

DIR *opendir(const char *name)
/* Open a directory for reading. */
{
	int d, f;
	DIR *dp;
	struct stat st;

	/* Only read directories. */
	if (stat(name, &st) < 0) return 0;
	if (!S_ISDIR(st.st_mode)) { errno= ENOTDIR; return 0; }

	if ((d= open(name, O_RDONLY | O_NONBLOCK)) < 0) return 0;

	/* Check the type again, mark close-on-exec, get a buffer. */
	if (fstat(d, &st) < 0
		|| (errno= ENOTDIR, !S_ISDIR(st.st_mode))
		|| (f= fcntl(d, F_GETFD)) < 0
		|| fcntl(d, F_SETFD, f | FD_CLOEXEC) < 0
		|| (dp= (DIR *) malloc(sizeof(*dp))) == 0
	) {
		int err= errno;
		(void) close(d);
		errno= err;
		return 0;
	}

	dp->_fd= d;
	dp->_count= 0;
	dp->_pos= 0;

	return dp;
}

