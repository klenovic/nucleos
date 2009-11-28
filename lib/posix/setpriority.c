/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
priority.c
*/

#include <nucleos/errno.h>
#include <nucleos/types.h>
#include <nucleos/resource.h>
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <nucleos/string.h>
#include <nucleos/stddef.h>

int setpriority(int which, int who, int prio)
{
	message m;

	m.m1_i1 = which;
	m.m1_i2 = who;
	m.m1_i3 = prio;

	return ksyscall(PM_PROC_NR, __NR_setpriority, &m);
}
