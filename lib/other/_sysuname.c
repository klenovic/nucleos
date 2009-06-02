/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sysuname(2) - transfer uname(3) strings.	Author: Kees J. Bot
 *								5 Dec 1992
 */

#define sysuname _sysuname
#include <lib.h>

int sysuname(int req, int field, char *value, size_t len)
{
	message m;

	m.m1_i1 = req;
	m.m1_i2 = field;
	m.m1_i3 = len;
	m.m1_p1 = value;

	/* Clear unused fields */
	m.m1_p2 = NULL;
	m.m1_p3 = NULL;

	return _syscall(MM, SYSUNAME, &m);
}

/*
 * $PchId: _sysuname.c,v 1.4 1995/11/27 19:42:09 philip Exp $
 */
