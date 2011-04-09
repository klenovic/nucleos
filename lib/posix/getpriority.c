/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <asm/syscall.h>

/* The return value of getpriority is biased by this value
 * to avoid returning negative values. */
#define PZERO	20

int getpriority(int which, int who)
{
	int res;

	res = INLINE_SYSCALL(getpriority, 2, which, who);

	if (res >= 0)
		res = PZERO - res;

	return res;
}
