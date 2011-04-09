/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <nucleos/signal.h>

/**
 * @brief
 * @param sp  where to put it
 */
int getsigset(sigset_t *sp)
{
	kipc_msg_t m;
	m.m_data1 = ENDPT_SELF;			/* request own signal set */

	if (ktaskcall(PM_PROC_NR, KCNR_PROCSTAT, &m) < 0)
		return(-1);

	*sp = m.m_data4;

	return(0);
}

