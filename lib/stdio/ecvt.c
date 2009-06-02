/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* $Header: /cvsup/minix/src/lib/stdio/ecvt.c,v 1.1.1.1 2005/04/21 14:56:35 beng Exp $ */

#ifndef NOFLOAT

#include	"../ansi/ext_fmt.h"
void _dbl_ext_cvt(double value, struct EXTEND *e);
char *_ext_str_cvt(struct EXTEND *e, int ndigit, int *decpt, int * sign, int ecvtflag);

static char *
cvt(long double value, int ndigit, int *decpt, int *sign, int ecvtflag)
{
	struct EXTEND e;

	_dbl_ext_cvt(value, &e);
	return _ext_str_cvt(&e, ndigit, decpt, sign, ecvtflag);
}

char *
_ecvt(long double value, int ndigit, int *decpt, int *sign)
{

	return cvt(value, ndigit, decpt, sign, 1);
}

char *
_fcvt(long double value, int ndigit, int *decpt, int *sign)
{
	return cvt(value, ndigit, decpt, sign, 0);
}

#endif	/* NOFLOAT */
