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
#include <asm/sigcontext.h>
#include <nucleos/signal.h>
#include <asm/syscall.h>

extern int __sigreturn(void);

int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	return INLINE_SYSCALL(sigaction, 4, sig, act, oact, __sigreturn);
}
