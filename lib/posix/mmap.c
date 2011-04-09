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
#include <nucleos/mman.h>
#include <asm/syscall.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	unsigned long buff[6];

	buff[0] = (unsigned long)addr;
	buff[1] = (unsigned long)len;
	buff[2] = (unsigned long)prot;
	buff[3] = (unsigned long)flags;
	buff[4] = (unsigned long)fd;
	buff[5] = (unsigned long)offset;

	return (void*)INLINE_SYSCALL(mmap, 1, buff);
}
