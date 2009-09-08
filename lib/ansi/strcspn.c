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

size_t
strcspn(const char *string, const char *notin)
{
	register const char *s1, *s2;

	for (s1 = string; *s1; s1++) {
		for(s2 = notin; *s2 != *s1 && *s2; s2++)
			/* EMPTY */ ;
		if (*s2)
			break;
	}
	return s1 - string;
}
