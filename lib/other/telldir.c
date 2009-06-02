/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	telldir()					Author: Kees J. Bot
 *								24 Apr 1989
 */
#define nil 0
#include <lib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

off_t telldir(DIR *dp)
/* Return the current read position in a directory. */
{
	struct dirent *dep;

	if (dp == nil) { errno= EBADF; return -1; }

	if (dp->_pos < dp->_count)
	{
		/* Use position in next entry */
		dep= (struct dirent *)&dp->_buf[dp->_pos];
		return dep->d_off;
	}

	/* Get current offset in directory */
	return lseek(dp->_fd, 0, SEEK_CUR);
}
