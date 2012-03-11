/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
newroot.c

Replace the current root with a new one
*/

#include <nucleos/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <nucleos/string.h>
#include <nucleos/mount.h>

void usage(void) {
	fprintf(stderr, "Usage: newroot [-i] <block-special>\n");
	fprintf(stderr, "-i: copy mfs binary from boot image to memory\n");
	exit(1);	
}

int main(int argc, char *argv[])
{
	int r;
	char *dev = 0;
	int mountflags;

	r = 0;
	mountflags = 0; /* !read-only */
	
	if (argc != 2 && argc != 3) usage();
	if(argc == 2) {
		dev = argv[1];
	} else if(argc == 3) {
		/* -i flag was supposedly entered. Verify.*/		
		if(strcmp(argv[1], "-i") != 0) usage();
		mountflags |= MS_REUSE;
		dev = argv[2];
	}

	r = mount(dev, "/", mountflags, "ext2", NULL);
	if (r != 0) {
		fprintf(stderr, "newroot: mount failed: %s\n",strerror(errno));
		exit(1);
	}

	return 0;
}
