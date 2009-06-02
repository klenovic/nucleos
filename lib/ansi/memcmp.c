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
/* $Id: memcmp.c,v 1.1.1.1 2005/04/21 14:56:05 beng Exp $ */

#include	<string.h>

int
memcmp(const void *s1, const void *s2, size_t n)
{
	register const unsigned char *p1 = s1, *p2 = s2;

	if (n) {
		n++;
		while (--n > 0) {
			if (*p1++ == *p2++) continue;
			return *--p1 - *--p2;
		}
	}
	return 0;
}
