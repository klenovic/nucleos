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
 * sscanf - read formatted output from a string
 */
/* $Header: /cvsup/minix/src/lib/stdio/sscanf.c,v 1.1.1.1 2005/04/21 14:56:36 beng Exp $ */

#include	<stdio.h>
#include	<stdarg.h>
#include	<string.h>
#include	"loc_incl.h"

int sscanf(const char *s, const char *format, ...)
{
	va_list ap;
	int retval;
	FILE tmp_stream;

	va_start(ap, format);

	tmp_stream._fd     = -1;
	tmp_stream._flags  = _IOREAD + _IONBF + _IOREADING;
	tmp_stream._buf    = (unsigned char *) s;
	tmp_stream._ptr    = (unsigned char *) s;
	tmp_stream._count  = strlen(s);

	retval = _doscan(&tmp_stream, format, ap);

	va_end(ap);

	return retval;
}
