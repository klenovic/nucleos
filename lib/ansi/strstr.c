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
strstr(register const char *s, register const char *wanted)
{
	register const size_t len = strlen(wanted);

	if (len == 0) return (char *)s;
	while (*s != *wanted || strncmp(s, wanted, len))
		if (*s++ == '\0')
			return (char *)NULL;
	return (char *)s;
}
