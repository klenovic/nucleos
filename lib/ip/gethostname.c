/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* gethostname(2) system call emulation */

#include <nucleos/types.h>
#include <nucleos/fcntl.h>
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <net/netdb.h>

#define HOSTNAME_FILE "/etc/hostname.file"

int gethostname(char *buf, size_t len)
{
	int fd;
	int r;
	char *nl;

	if ((fd= open(HOSTNAME_FILE, O_RDONLY)) < 0) return -1;

	r= read(fd, buf, len);
	close(fd);
	if (r == -1) return -1;

	buf[len-1]= '\0';
	if ((nl= strchr(buf, '\n')) != NULL) *nl= '\0';
	return 0;
}
