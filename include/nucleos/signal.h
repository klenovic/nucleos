/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_SIGNAL_H
#define __NUCLEOS_SIGNAL_H

#include <nucleos/types.h>
#include <asm/signal.h>
#include <asm/siginfo.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)

/* POSIX and ANSI function prototypes. */
int raise(int _sig);
__sighandler_t signal(int _sig, __sighandler_t _func);

int kill(pid_t _pid, int _sig);
int killpg(pid_t _pgrp, int _sig);
int sigaction(int _sig, const struct sigaction *_act, struct sigaction *_oact);
int sigpending(sigset_t *_set);
int sigprocmask(int _how, const sigset_t *_set, sigset_t *_oset);
int sigsuspend(const sigset_t *_sigmask);

#define sigaddset(set, sig)	((int) ((*(set) |= (1 << (sig))) && 0))
#define sigdelset(set, sig)	((int) ((*(set) &= ~(1 << (sig))) && 0))
#define sigemptyset(set)	((int) (*(set) = 0))
#define sigfillset(set)		((int) ((*(set) = ~0) && 0))
#define sigismember(set, sig)	((*(set) & (1 << (sig))) ? 1 : 0)
#else
/* @nucleos: Probably useless since we don't wan't user programs but for now
	     it used for testing. */

/* For the sigset functions, only use the library version with error
 * checking from user programs. System programs need to be able to use
 * nonstanard signals.
 */
int sigaddset(sigset_t *_set, int _sig);
int sigdelset(sigset_t *_set, int _sig);
int sigemptyset(sigset_t *_set);
int sigfillset(sigset_t *_set);
int sigismember(const sigset_t *_set, int _sig);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_SIGNAL_H */
