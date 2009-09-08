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
nice.c
*/

#include <nucleos/errno.h>
#include <nucleos/unistd.h>
#include <nucleos/resource.h>

int nice(incr)
int incr;
{
	int r;

	errno= 0;
	r= getpriority(PRIO_PROCESS, 0);
	if (r == -1 && errno != 0)
		return r;
	return setpriority(PRIO_PROCESS, 0, r+incr);
}
