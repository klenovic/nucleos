/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <nucleos/errno.h>
#include <asm/syscall.h>

extern void *__curbrk;

int brk(void *addr)
{
	void *newbrk = (void*)INLINE_SYSCALL(brk, 1, addr);

	__curbrk = newbrk;

	if (newbrk < addr) {
		__set_errno (ENOMEM);
		return -1;
	}

	return 0;
}
