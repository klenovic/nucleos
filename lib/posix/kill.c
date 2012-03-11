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

/**
 * @brief kill
 * @param proc  which process is to be sent the signal
 * @param sig  signal number
 */
int kill(int proc, int sig)
{
	return INLINE_SYSCALL(kill, 2, proc, sig);
}
