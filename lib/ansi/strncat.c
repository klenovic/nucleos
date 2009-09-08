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

char *
strncat(char *ret, register const char *s2, size_t n)
{
	register char *s1 = ret;

	if (n > 0) {
		while (*s1++)
			/* EMPTY */ ;
		s1--;
		while (*s1++ = *s2++)  {
			if (--n > 0) continue;
			*s1 = '\0';
			break;
		}
		return ret;
	} else return s1;
}
