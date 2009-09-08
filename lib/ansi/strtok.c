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
strtok(register char *string, const char *separators)
{
	register char *s1, *s2;
	static char *savestring;

	if (string == NULL) {
		string = savestring;
		if (string == NULL) return (char *)NULL;
	}

	s1 = string + strspn(string, separators);
	if (*s1 == '\0') {
		savestring = NULL;
		return (char *)NULL;
	}

	s2 = strpbrk(s1, separators);
	if (s2 != NULL)
		*s2++ = '\0';
	savestring = s2;
	return s1;
}
