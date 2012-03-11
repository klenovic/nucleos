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
getpagesize.c
*/

#include <nucleos/unistd.h>

size_t getpagesize(void)
{
	/* We don't have paging. Pretend that we do. */
	return 4096;
}
