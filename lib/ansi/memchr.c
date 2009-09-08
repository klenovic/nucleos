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
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include	<nucleos/string.h>

void *
memchr(const void *s, register int c, register size_t n)
{
	register const unsigned char *s1 = s;

	c = (unsigned char) c;
	if (n) {
		n++;
		while (--n > 0) {
			if (*s1++ != c) continue;
			return (void *) --s1;
		}
	}
	return NULL;
}
