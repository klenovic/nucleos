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
#include <nucleos/wait.h>
#include <asm/syscall.h>

pid_t waitpid(pid_t pid, int *status, int options)
{
	return INLINE_SYSCALL(waitpid, 3, pid, status, options);
}
