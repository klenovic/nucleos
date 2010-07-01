/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * printf - write on the standard output stream
 */
#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
printf(const char *format, ...)
{
	va_list ap;
	int retval;

	va_start(ap, format);

	retval = _doprnt(format, ap, stdout);

	va_end(ap);

	return retval;
}
