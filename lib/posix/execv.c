/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	execv() - execute with prepared arguments	Author: Kees J. Bot
 *								21 Jan 1994
 */
#include <nucleos/unistd.h>

extern char * const **_penviron;	/* The default environment. */

int execv(const char *path, char * const *argv)
{
	return execve(path, argv, *_penviron);
}
