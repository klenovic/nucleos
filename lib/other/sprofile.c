/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <asm/syscall.h>

int sprofile(int action, int size, int freq, char *ctl_ptr, int *mem_ptr)
{
	return INLINE_SYSCALL(sprof, 5, action, size, freq, ctl_ptr, mem_ptr);
}
