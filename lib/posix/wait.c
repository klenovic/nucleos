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
#include <nucleos/wait.h>

pid_t wait(int *status)
{
	message m;

	if (_syscall(PM_PROC_NR, __NR_wait, &m) < 0)
		return(-1);

	if (status != 0)
		*status = m.m2_i1;

	return(m.m_type);
}
