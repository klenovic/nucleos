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
#include <nucleos/signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	message m;

	if (set == (sigset_t *) NULL) {
		m.m2_i1 = SIG_INQUIRE;
		m.m2_l1 = 0;
	} else {
		m.m2_i1 = how;
		m.m2_l1 = (long) *set;
	}

	if (ksyscall(PM_PROC_NR, __NR_sigprocmask, &m) < 0)
		return(-1);

	if (oset != (sigset_t *) NULL)
		*oset = (sigset_t) (m.m2_l1);

	return(m.m_type);
}
