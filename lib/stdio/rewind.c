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
 * rewind.c - set the file position indicator of a stream to the start
 */
#include	<stdio.h>
#include	"loc_incl.h"

void
rewind(FILE *stream)
{
	(void) fseek(stream, 0L, SEEK_SET);
	clearerr(stream);
}
