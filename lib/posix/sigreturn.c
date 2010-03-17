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
#include <asm/sigcontext.h>
#include <nucleos/signal.h>

int sigreturn(register struct sigcontext *scp)
{
	sigset_t set;

	/* The message can't be on the stack, because the stack will vanish out
	 * from under us.  The send part of sendrec will succeed, but when
	 * a message is sent to restart the current process, who knows what will
	 * be in the place formerly occupied by the message?
	 */
	int resultvar = 0;
	static int __msg[9];

	__msg[1] = __NR_sigreturn;

	/* Protect against race conditions by blocking all interrupts. */
	sigfillset(&set);		/* splhi */
	sigprocmask(SIG_SETMASK, &set, (sigset_t *) NULL);

	__asm__ __volatile__(ASM_SYSCALL_CALL_SYSTEM
		: "=a" (resultvar)
		: "0"(__msg), "b"(scp), "c"(scp->sc_mask), "d"(scp->sc_flags)
		: "memory", "cc"
	);

	if (__builtin_expect(INTERNAL_SYSCALL_ERROR_P(resultvar, ), 0)) {
		__set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));
		return -1;
	}

	return resultvar;
}
