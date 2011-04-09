/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * perror.c - print an error message on the standard error output
 */
#include <nucleos/types.h>
#include <stdio.h>
#include <nucleos/errno.h>
#include <stdio.h>
#include <nucleos/string.h>
#include "loc_incl.h"

ssize_t write(int d, const char *buf, size_t nbytes);

void
perror(const char *s)
{
	char *p;
	int fd;

	p = strerror(errno);
	fd = fileno(stderr);
	fflush(stdout);
	fflush(stderr);
	if (s && *s) {
		write(fd, s, strlen(s));
		write(fd, ": ", 2);
	}
	write(fd, p, strlen(p));
	write(fd, "\n", 1);
}
