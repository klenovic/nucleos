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
 * setlocale - set the programs locale
 */
#include	<locale.h>
#include	<nucleos/string.h>

struct lconv _lc;

char *
setlocale(int category, const char *locale)
{
	if (!locale) return "C";
	if (*locale && strcmp(locale, "C")) return (char *)NULL;
	
	switch(category) {
	case LC_ALL:
	case LC_CTYPE:
	case LC_COLLATE:
	case LC_TIME:
	case LC_NUMERIC:
	case LC_MONETARY:
		return *locale ? (char *)locale : "C";
	default:
		return (char *)NULL;
	}
}
