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
/* System processes use simpler macros with no range error checking (defined in
 * signal.h). The ANSI signal() implementation now also uses the macro
 * versions, which makes hiding of the functions here a historical remains.
 */
#include <nucleos/signal.h>

/* Low bit of signal masks. */
#define SIGBIT_0	((sigset_t) 1)

/* Mask of valid signals (0 - _NSIG). */
#define SIGMASK		(((SIGBIT_0 << _NSIG) << 1) - 1)

#define sigisvalid(signo) ((unsigned) (signo) <= _NSIG)
/*@nucleos: remove this*/
#if 0
int sigaddset(sigset_t *set, int signo)
{
	if (!sigisvalid(signo)) {
		errno = EINVAL;
		return -1;
	}

	*set |= SIGBIT_0 << signo;
	return 0;
}

int sigdelset(sigset_t *set, int signo)
{
	if (!sigisvalid(signo)) {
		errno = EINVAL;
		return -1;
	}

	*set &= ~(SIGBIT_0 << signo);

	return 0;
}

int sigemptyset(sigset_t *set)
{
	*set = 0;
	return 0;
}

int sigfillset(sigset_t *set)
{
	*set = SIGMASK;
	return 0;
}

int sigismember(const sigset_t *set, int signo)
{
	if (!sigisvalid(signo)) {
		errno = EINVAL;
		return -1;
	}

	if (*set & (SIGBIT_0 << signo))
		return 1;

	return 0;
}
#endif
