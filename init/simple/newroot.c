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
newroot.c

Replace the current root with a new one
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int r;
	char *dev;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: newroot <block-special>\n");
		exit(1);
	}
	dev = argv[1];
	r = mount(dev, "/", 0 /* !ro */, NULL, NULL);
	if (r != 0)
	{
		fprintf(stderr, "newroot: mount failed: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}
