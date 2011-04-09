/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	execl() - execute				Author: Kees J. Bot
 *								21 Jan 1994
 */
#include <nucleos/unistd.h>

extern char * const **_penviron;	/* The default environment. */

int execl(const char *path, const char *arg1, ...)
/* execl("/bin/sh", "sh", "-c", "example", (char *) 0); */
{
	/* Assumption:  The C-implementation for this machine pushes
	 * function arguments downwards on the stack making a perfect
	 * argument array.  Luckily this is almost always so.
	 */
	return execve(path, (char * const *) &arg1, *_penviron);
}
