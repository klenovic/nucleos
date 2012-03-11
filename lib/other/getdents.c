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
#include <nucleos/dirent.h>
#include <asm/syscall.h>

ssize_t getdents(int fd, struct dirent *buffer, size_t nbytes)
{
	return INLINE_SYSCALL(getdents, 3, fd, buffer, nbytes);
}
