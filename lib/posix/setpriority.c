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

int setpriority(int which, int who, int prio)
{
	/* We have to scale prio between MIN_USER_Q and MAX_USER_Q to match
	 * the kernel's scheduling queues.
	 * The final nice value is:
	 * niceval = MAX_USER_Q + (prio - PRIO_MIN) * (MIN_USER_Q - MAX_USER_Q + 1) /
	 *           (PRIO_MAX-PRIO_MIN+1);
	 */

	return INLINE_SYSCALL(setpriority, 3, which, who, prio);
}
