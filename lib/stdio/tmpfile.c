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
 * tmpfile.c - create and open a temporary file
 */
#include <nucleos/types.h>
#include <stdio.h>
#include <nucleos/string.h>
#include "loc_incl.h"

pid_t getpid(void);

FILE *
tmpfile(void) {
	static char name_buffer[L_tmpnam] = "/tmp/tmp." ;
	static char *name = NULL;
	FILE *file;

	if (!name) {
		name = name_buffer + strlen(name_buffer);
		name = _i_compute(getpid(), 10, name, 5);
		*name = '\0';
	}

	file = fopen(name_buffer,"wb+");
	if (!file) return (FILE *)NULL;
	(void) remove(name_buffer);
	return file;
}
