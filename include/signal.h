/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <signal.h> header defines all the ANSI and POSIX signals.
 * MINIX supports all the signals required by POSIX. They are defined below.
 * Some additional signals are also supported.
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <nucleos/types.h>

/* Here are types that are closely associated with signal handling. */
typedef int sig_atomic_t;

#ifndef _SIGSET_T
#define _SIGSET_T
typedef unsigned long sigset_t;
#endif

/* Regular signals. */
#define SIGHUP             1	/* hangup */
#define SIGINT             2	/* interrupt (DEL) */
#define SIGQUIT            3	/* quit (ASCII FS) */
#define SIGILL             4	/* illegal instruction */
#define SIGTRAP            5	/* trace trap (not reset when caught) */
#define SIGABRT            6	/* IOT instruction */
#define SIGBUS             7	/* bus error */
#define SIGFPE             8	/* floating point exception */
#define SIGKILL            9	/* kill (cannot be caught or ignored) */
#define SIGUSR1           10	/* user defined signal # 1 */
#define SIGSEGV           11	/* segmentation violation */
#define SIGUSR2           12	/* user defined signal # 2 */
#define SIGPIPE           13	/* write on a pipe with no one to read it */
#define SIGALRM           14	/* alarm clock */
#define SIGTERM           15	/* software termination signal from kill */
#define SIGEMT		  16	/* EMT instruction */
#define SIGCHLD           17	/* child process terminated or stopped */
#define SIGWINCH    	  21	/* window size has changed */

/* POSIX requires the following signals to be defined, even if they are
 * not supported.  Here are the definitions, but they are not supported.
 */
#define SIGCONT           18	/* continue if stopped */
#define SIGSTOP           19	/* stop signal */
#define SIGTSTP           20	/* interactive stop signal */
#define SIGTTIN           22	/* background process wants to read */
#define SIGTTOU           23	/* background process wants to write */

#define _NSIG             24	/* highest signal number plus one */

#define SIGIOT             SIGABRT /* for people who speak PDP-11 */

/* MINIX specific signals. These signals are not used by user proceses, 
 * but meant to inform system processes, like the PM, about system events.
 */
#define SIGKMESS   	  29	/* new kernel message */
#define SIGKSIG    	  30	/* kernel signal pending */
#define SIGKSTOP    	  31	/* kernel shutting down */

typedef void (*__sighandler_t)(int);

/* Macros used as function pointers. */
#define SIG_ERR    ((__sighandler_t) -1)	/* error return */
#define SIG_DFL	   ((__sighandler_t)  0)	/* default signal handling */
#define SIG_IGN	   ((__sighandler_t)  1)	/* ignore signal */
#define SIG_HOLD   ((__sighandler_t)  2)	/* block signal */
#define SIG_CATCH  ((__sighandler_t)  3)	/* catch signal */
#define SIG_MESS   ((__sighandler_t)  4)	/* pass as message (MINIX) */

struct sigaction {
  __sighandler_t sa_handler;	/* SIG_DFL, SIG_IGN, or pointer to function */
  sigset_t sa_mask;		/* signals to be blocked during handler */
  int sa_flags;			/* special flags */
};

/* Fields for sa_flags. */
#define SA_ONSTACK   0x0001	/* deliver signal on alternate stack */
#define SA_RESETHAND 0x0002	/* reset signal handler when signal caught */
#define SA_NODEFER   0x0004	/* don't block signal while catching it */
#define SA_RESTART   0x0008	/* automatic system call restart */
#define SA_SIGINFO   0x0010	/* extended signal handling */
#define SA_NOCLDWAIT 0x0020	/* don't create zombies */
#define SA_NOCLDSTOP 0x0040	/* don't receive SIGCHLD when child stops */

/* POSIX requires these values for use with sigprocmask(2). */
#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */
#define SIG_INQUIRE        4	/* for internal use only */

/* POSIX and ANSI function prototypes. */
int raise(int _sig);
__sighandler_t signal(int _sig, __sighandler_t _func);

int kill(pid_t _pid, int _sig);
int killpg(pid_t _pgrp, int _sig);
int sigaction(int _sig, const struct sigaction *_act, struct sigaction *_oact);
int sigaddset(sigset_t *_set, int _sig);
int sigdelset(sigset_t *_set, int _sig);
int sigemptyset(sigset_t *_set);
int sigfillset(sigset_t *_set);
int sigismember(const sigset_t *_set, int _sig);
int sigpending(sigset_t *_set);
int sigprocmask(int _how, const sigset_t *_set, sigset_t *_oset);
int sigsuspend(const sigset_t *_sigmask);

#endif /* _SIGNAL_H */
