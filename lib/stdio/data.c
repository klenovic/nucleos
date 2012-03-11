/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * data.c - this is the initialization for the standard streams
 */
#include	<stdio.h>

struct __iobuf __stdin = {
	0, 0, _IOREAD, 0,
	(unsigned char *)NULL, (unsigned char *)NULL, 
};

struct __iobuf __stdout = {
	0, 1, _IOWRITE, 0,
	(unsigned char *)NULL, (unsigned char *)NULL, 
};

struct __iobuf __stderr = {
	0, 2, _IOWRITE | _IOLBF, 0,
	(unsigned char *)NULL, (unsigned char *)NULL, 
};

FILE *__iotab[FOPEN_MAX] = {
	&__stdin,
	&__stdout,
	&__stderr,
	0
};
