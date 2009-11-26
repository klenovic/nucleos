/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* utime(2) for POSIX		Authors: Terrence W. Holm & Edwin L. Froese */

#include <nucleos/unistd.h>
#include <nucleos/utime.h>
#include <asm/syscall.h>

int utime(const char *filename, const struct utimbuf *times)
{
	return INLINE_SYSCALL(utime, 2, filename, times);
}
