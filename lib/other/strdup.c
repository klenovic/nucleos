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
lib/other/strdup.c
*/

#include <stdlib.h>
#include <nucleos/string.h>

char *strdup(s1)
const char *s1;
{
	size_t len;
	char *s2;

	len= strlen(s1)+1;

	s2= malloc(len);
	if (s2 == NULL)
		return NULL;
	strcpy(s2, s1);

	return s2;
}

