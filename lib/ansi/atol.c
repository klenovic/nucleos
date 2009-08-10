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
#include	<ctype.h>
#include	<stdlib.h>

/* We do not use strtol here for backwards compatibility in behaviour on
   overflow.
*/
long
atol(register const char *nptr)
{
	long total = 0;
	int minus = 0;

	while (isspace(*nptr)) nptr++;
	if (*nptr == '+') nptr++;
	else if (*nptr == '-') {
		minus = 1;
		nptr++;
	}
	while (isdigit(*nptr)) {
		total *= 10;
		total += (*nptr++ - '0');
	}
	return minus ? -total : total;
}
