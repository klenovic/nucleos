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
libc/ieee_float/isinf.c

Created:	Oct 14, 1993 by Philip Homburg <philip@cs.vu.nl>

Implementation of isinf that directly tests the bits in an ieee float
*/
#include <nucleos/types.h>
#include <math.h>

#include "ieee_float.h"

int isinf(value)
double value;
{
	struct f64 *f64p;
	int exp;

	f64p= (struct f64 *)&value;
	exp= F64_GET_EXP(f64p);
	if (exp != F64_EXP_MAX)
		return 0;
	return F64_GET_MANT_LOW(f64p) == 0 && F64_GET_MANT_HIGH(f64p) == 0;
}
