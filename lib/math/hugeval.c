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
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Eck
 */
/* $Header: /cvsup/minix/src/lib/math/hugeval.c,v 1.1.1.1 2005/04/21 14:56:26 beng Exp $ */
#include	<math.h>

double
__huge_val(void)
{
	return 1.0e+1000;	/* This will generate a warning */
}
