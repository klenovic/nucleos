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
lib/posix/usleep.c
*/

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

int usleep(useconds_t useconds)
{
	int r;
	struct timeval tv;

	tv.tv_sec= useconds/1000000;
	tv.tv_usec= useconds % 1000000;
	r= select(0, NULL, NULL, NULL, &tv);
	return r;
}
