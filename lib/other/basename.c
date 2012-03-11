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
basename.c
*/

#include <libgen.h>
#include <nucleos/string.h>

char *basename(path)
char *path;
{
	size_t len;
	char *cp;

	if (path == NULL)
		return ".";
	len= strlen(path);
	if (len == 0)
		return ".";
	while (path[len-1] == '/')
	{
		if (len == 1)
			return path;	/* just "/" */
		len--;
		path[len]= '\0';
	}
	cp= strrchr(path, '/');
	if (cp != NULL)
		return cp+1;
	return path;
}
