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
 * puts.c - print a string onto the standard output stream
 */
#include	<stdio.h>

int
puts(register const char *s)
{
	register FILE *file = stdout;
	register int i = 0;

	while (*s) {
		if (putc(*s++, file) == EOF) return EOF;
		else i++;
	}
	if (putc('\n', file) == EOF) return EOF;
	return i + 1;
}
