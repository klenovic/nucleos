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
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <nucleos/types.h>
#include <nucleos/signal.h>
#include <stdlib.h>

extern void (*_clean)(void);

void
abort(void)
{
	if (_clean) _clean();		/* flush all output files */
	raise(SIGABRT);
	exit(-1);
	/* NORETURN */
}

