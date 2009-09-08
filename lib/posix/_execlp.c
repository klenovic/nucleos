/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	execlp() - execute with PATH search		Author: Kees J. Bot
 *								22 Jan 1994
 */
#include <nucleos/unistd.h>

int execlp(const char *file, const char *arg1, ...)
/* execlp("sh", "sh", "-c", "example", (char *) 0); */
{
	return execvp(file, (char * const *) &arg1);
}
