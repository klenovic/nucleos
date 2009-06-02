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
 * putw - write an word on a stream
 */
/* $Header$ */

#include	<stdio.h>

_PROTOTYPE(int putw, (int w, FILE *stream ));

int
putw(w, stream)
int w;
register FILE *stream;
{
	register int cnt = sizeof(int);
	register char *p = (char *) &w;

	while (cnt--) {
		putc(*p++, stream);
	}
	if (ferror(stream)) return EOF;
	return w;
}
