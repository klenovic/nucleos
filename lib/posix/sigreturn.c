/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <asm/sigcontext.h>
#include <nucleos/signal.h>

int sigreturn(register struct sigcontext *scp)
{
	sigset_t set;

	/* Protect against race conditions by blocking all interrupts. */
	sigfillset(&set);		/* splhi */
	sigprocmask(SIG_SETMASK, &set, (sigset_t *)NULL);

	return INLINE_SYSCALL(sigreturn, 3, scp, scp->sc_mask, scp->sc_flags);
}
