/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * remove.c - remove a file
 */
#include <stdio.h>
#include <nucleos/errno.h>

int rmdir(const char *path);
int unlink(const char *path);

int
remove(const char *filename) {
	int saved_errno, retval;

	saved_errno = errno;

	retval = rmdir(filename);

	if (retval == -1 && errno == ENOTDIR) {
		errno = saved_errno;

		retval = unlink(filename);
	}

	return retval;
}
