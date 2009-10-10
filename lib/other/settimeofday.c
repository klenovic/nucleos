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
settimeofday.c
*/
#include <nucleos/time.h>

int settimeofday(const struct timeval *tp, const void *tzp)
{
	/* Ignore time zones */
	return stime((time_t*)&tp->tv_sec);
}
