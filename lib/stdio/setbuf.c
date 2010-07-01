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
 * setbuf.c - control buffering of a stream
 */
#include	<stdio.h>
#include	"loc_incl.h"

void
setbuf(register FILE *stream, char *buf)
{
	(void) setvbuf(stream, buf, (buf ? _IOFBF : _IONBF), (size_t) BUFSIZ);
}
