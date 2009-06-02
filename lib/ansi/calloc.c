/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* $Header: /cvsup/minix/src/lib/ansi/calloc.c,v 1.1.1.1 2005/04/21 14:56:04 beng Exp $ */
#include	<stdlib.h>

#define ALIGN(x)	(((x) + (sizeof(size_t) - 1)) & ~(sizeof(size_t) - 1))

void *
calloc(size_t nelem, size_t elsize)
{
	register char *p;
	register size_t *q;
	size_t size = ALIGN(nelem * elsize);

	p = malloc(size);
	if (p == NULL) return NULL;
	q = (size_t *) (p + size);
	while ((char *) q > p) *--q = 0;
	return p;
}

