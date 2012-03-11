/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <asm/syscall.h>

loff_t lseek64(int fd, loff_t offset, int whence)
{
	loff_t result;
	return (loff_t)(INLINE_SYSCALL(llseek, 5, fd, (off_t) (offset >> 32),
			(off_t) (offset & 0xffffffff), &result, whence) ? : result);
}
