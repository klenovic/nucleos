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
/* $Header: /cvsup/minix/src/lib/math/log10.c,v 1.1.1.1 2005/04/21 14:56:26 beng Exp $ */

#include	<math.h>
#include	<errno.h>
#include	"localmath.h"

double
log10(double x)
{
	if (__IsNan(x)) {
		errno = EDOM;
		return x;
	}
	if (x < 0) {
		errno = EDOM;
		return -HUGE_VAL;
	}
	else if (x == 0) {
		errno = ERANGE;
		return -HUGE_VAL;
	}

	return log(x) / M_LN10;
}
