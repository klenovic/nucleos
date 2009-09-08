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
domainname.c
*/

#include <stdio.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <net/netlib.h>

int getdomainname(domain, size)
char *domain;
size_t size;
{
	FILE *domainfile;
	char *line;

	domainfile= fopen("/etc/domainname", "r");
	if (!domainfile)
	{
		return -1;
	}

	line= fgets(domain, size, domainfile);
	fclose(domainfile);
	if (!line)
		return -1;
	line= strchr(domain, '\n');
	if (line)
		*line= '\0';
	return 0;
}
