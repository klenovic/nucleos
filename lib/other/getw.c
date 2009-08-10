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
 * getw - read a word from a stream
 */
#include	<stdio.h>

int getw(FILE *stream);

int getw(stream)
register FILE *stream;
{
	register int cnt = sizeof(int);
	int w;
	register char *p = (char *) &w;

	while (cnt--) {
		*p++ = getc(stream);
	}
	if (feof(stream) || ferror(stream)) return EOF;
	return w;
}
