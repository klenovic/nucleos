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
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Eck
 */
#include	<math.h>
#include	<errno.h>

double
fmod(double x, double y)
{
	long	i;
	double val;
	double frac;

	if (y == 0) {
		errno = EDOM;
		return 0;
	}
	frac = modf( x / y, &val);

	return frac * y;

/*
	val = x / y;
	if (val > LONG_MIN && val < LONG_MAX) {
		i = val;
		return x - i * y;
	}
*/
}
