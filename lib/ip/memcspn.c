/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

size_t
memcspn(const char *string, size_t strlen, const char *notin, size_t notinlen)
{
	register const char *s1, *s2;
	int i,j;

	for (s1 = string, i = 0; i<strlen; s1++, i++) {
		for(s2 = notin, j = 0; *s2 != *s1 && j < notinlen; s2++, j++)
			/* EMPTY */ ;
		if (j != notinlen)
			break;
	}
	return s1 - string;
}
