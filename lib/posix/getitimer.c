/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Copyright (c) 2009 Vrije Universiteit, Amsterdam.
 * See the copyright notice in file LICENSE.minix3.
 */
#include <nucleos/lib.h>
#include <nucleos/time.h>

/*
 * This is the implementation for the function to
 * invoke the interval timer retrieval system call.
 */
int getitimer(int which, struct itimerval *value)
{
	message m;

	m.m1_i1 = which;
	m.m1_p1 = NULL;		/* only retrieve the timer */
	m.m1_p2 = (char *) value;

	return ksyscall(PM_PROC_NR, __NR_itimer, &m);
}
