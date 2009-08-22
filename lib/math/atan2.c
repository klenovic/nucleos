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
 * Author: Ceriel J.H. Jacobs
 */
#include	<math.h>
#include	<nucleos/errno.h>
#include	"localmath.h"

double
atan2(double y, double x)
{
	double absx, absy, val;

	if (x == 0 && y == 0) {
		errno = EDOM;
		return 0;
	}
	absy = y < 0 ? -y : y;
	absx = x < 0 ? -x : x;
	if (absy - absx == absy) {
		/* x negligible compared to y */
		return y < 0 ? -M_PI_2 : M_PI_2;
	}
	if (absx - absy == absx) {
		/* y negligible compared to x */
		val = 0.0;
	}
	else	val = atan(y/x);
	if (x > 0) {
		/* first or fourth quadrant; already correct */
		return val;
	}
	if (y < 0) {
		/* third quadrant */
		return val - M_PI;
	}
	return val + M_PI;
}
