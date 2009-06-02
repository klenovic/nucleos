/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	"../stdio/loc_incl.h"

int _fp_hook = 1;

char *
_f_print(va_list *ap, int flags, char *s, char c, int precision)
{
	fprintf(stderr,"cannot print floating point\n");
	exit(EXIT_FAILURE);
}
