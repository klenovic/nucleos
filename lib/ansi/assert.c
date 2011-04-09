/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include	<assert.h>
#include	<stdio.h>
#include	<stdlib.h>

void __bad_assertion(const char *mess) {

	fputs(mess, stderr);
	abort();
}
