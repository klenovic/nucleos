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
 * fgetpos.c - get the position in the file
 */
/* $Header: /cvsup/minix/src/lib/stdio/fgetpos.c,v 1.1.1.1 2005/04/21 14:56:35 beng Exp $ */

#include	<stdio.h>

int
fgetpos(FILE *stream, fpos_t *pos)
{
	*pos = ftell(stream);
	if (*pos == -1) return -1;
	return 0;
}
