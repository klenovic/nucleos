/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

void _exit(int status)
{
	void (*suicide)(void);
	message m;

	m.m1_i1 = status;
	ksyscall(PM_PROC_NR, __NR_exit, &m);

	/* If exiting nicely through PM fails for some reason, try to
	 * commit suicide. E.g., message to PM might fail due to deadlock.
	 */
	suicide = (void (*)(void)) -1;
	suicide();

	/* If committing suicide fails for some reason, hang. */
	for(;;);
}

