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
 * vscanf.c - read formatted input from the standard input stream
 */

#include	<stdio.h>
#include	<stdarg.h>
#include	"loc_incl.h"

int
vscanf(const char *format, va_list ap)
{
	return _doscan(stdin, format, ap);
}
