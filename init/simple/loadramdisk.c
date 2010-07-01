/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
loadramdisk.c

Copy a device or file specified as argument to /dev/ram
*/

#include <nucleos/errno.h>
#include <nucleos/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/ioctl.h>
#include <asm/ioctls.h>

#define RAM	"/dev/ram"

char buf[10240];

static unsigned long size_device(int fd);

int main(int argc, char *argv[])
{
	unsigned long off, size;
	int r, s, fd, ramfd;
	char *src;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: loadramdisk <file>\n");
		exit(1);
	}
	src= argv[1];
	fd= open(src, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Unable to open '%s': %s\n",
			src, strerror(errno));
		exit(1);
	}

	/* Get the size of the device */
	errno= 0;
	size= size_device(fd);
	if (errno != 0)
	{
		fprintf(stderr, "Lseek(end) failed on '%s': %s\n",
			src, strerror(errno));
		exit(1);
	}
	if (lseek(fd, 0, SEEK_SET) != 0)
	{
		fprintf(stderr, "Lseek(0) failed on '%s': %s\n",
			src, strerror(errno));
		exit(1);
	}

	ramfd= open(RAM, O_RDWR);
	if (ramfd < 0)
	{
		fprintf(stderr, "Unable to open '%s': %s\n",
			RAM, strerror(errno));
		exit(1);
	}
	r= ioctl(ramfd, MIOCRAMSIZE, &size);
	if (r != 0)
	{
		fprintf(stderr, "MIOCRAMSIZE %lu failed on '%s': %s\n",
			size, RAM, strerror(errno));
		exit(1);
	}

	off= 0;
	while (off < size)
	{
		r= read(fd, buf, sizeof(buf));
		if (r <= 0)
		{
			fprintf(stderr, "error reading '%s': %s\n",
				src, r == 0 ? "unexpected EOF" :
				strerror(errno));
			exit(1);
		}
		s= write(ramfd, buf, r);
		if (s != r)
		{
			fprintf(stderr, "error writing to '%s': %s\n",
				s >= 0 ? "short write" : strerror(errno));
			exit(1);
		}
		off += r;
	}
	exit(0);
}

static unsigned long size_device(int fd)
{
	char b;
	int r;
	unsigned long low, mid, high;

	/* Try to find the size of a device using binary search */
	low= 0;
	high= -1;

	while (mid= low+(high-low)/2, mid > low)
	{
		if (lseek(fd, mid, SEEK_SET) != mid)
		{
			fprintf(stderr, "lseek to %lu failed: %s\n",
				mid, strerror(errno));
			exit(1);
		}
		r= read(fd, &b, 1);
		if (r < 0)
		{
			fprintf(stderr, "read failed at position %lu: %s\n",
				mid, strerror(errno));
			exit(1);
		}
		if (r > 0)
			low= mid;
		else
			high= mid;
	}
	return high;
}
