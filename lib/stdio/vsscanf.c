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
 * vsscanf - read formatted output from a string
 */

#include	<stdio.h>
#include	<stdarg.h>
#include	<nucleos/string.h>
#include	"loc_incl.h"

int vsscanf(const char *s, const char *format, va_list ap)
{
	FILE tmp_stream;

	tmp_stream._fd     = -1;
	tmp_stream._flags  = _IOREAD + _IONBF + _IOREADING;
	tmp_stream._buf    = (unsigned char *) s;
	tmp_stream._ptr    = (unsigned char *) s;
	tmp_stream._count  = strlen(s);

	return _doscan(&tmp_stream, format, ap);
}
