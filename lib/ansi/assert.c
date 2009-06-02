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
 * assert.c - diagnostics
 */
/* $Header: /cvsup/minix/src/lib/ansi/assert.c,v 1.1.1.1 2005/04/21 14:56:04 beng Exp $ */

#include	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>

void __bad_assertion(const char *mess) {

	fputs(mess, stderr);
	abort();
}
