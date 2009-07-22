/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	getdomainname()					Author: Kees J. Bot
 *								2 Dec 1994
 */
#define nil 0
#include <nucleos/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>

int getdomainname(char *domain, size_t size)
{
	char nodename[256];
	char *dot;

	if (gethostname(nodename, sizeof(nodename)) < 0)
		return -1;
	nodename[sizeof(nodename)-1]= 0;
	if ((dot= strchr(nodename, '.')) == nil) dot= ".";

	strncpy(domain, dot+1, size);
	if (size > 0) domain[size-1]= 0;
	return 0;
}
