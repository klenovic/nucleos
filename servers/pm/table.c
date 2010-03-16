/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the table used to map system call numbers onto the
 * routines that perform them.
 */
#include "pm.h"
#include <nucleos/unistd.h>
#include <nucleos/signal.h>
#include <servers/pm/mproc.h>
#include "param.h"

#define SCALL_HANDLER(syscall, handler) \
	[ __NNR_ ## syscall ] = handler

int (*call_vec[])(void) = {
	[__NR_getsysinfo]	= do_getsysinfo,	/* 79 = getsysinfo */
	[__NR_getsysinfo_up]	= do_getsysinfo_up,/* 97 = getsysinfo_up */
	[__NR_ptrace]		= do_trace,
	[__NR_svrctl]		= do_svrctl,	/* 77 = svrctl	*/
	[KCNR_ADDDMA]		= do_adddma,	/* 108 = adddma */
	[KCNR_DELDMA]		= do_deldma,	/* 109 = deldma */
	[KCNR_GETDMA]		= do_getdma,	/* 110 = getdma */

	/* Nucleos syscalls */
	SCALL_HANDLER(alarm,		do_alarm),
	SCALL_HANDLER(brk,		scall_brk),
	SCALL_HANDLER(cprof,		do_cprofile),
	SCALL_HANDLER(exec,		do_exec),
	SCALL_HANDLER(exit,		do_exit),
	SCALL_HANDLER(fork,		do_fork),
	SCALL_HANDLER(getegid,		scall_getegid),
	SCALL_HANDLER(geteuid,		scall_geteuid),
	SCALL_HANDLER(getgid,		scall_getgid),
	SCALL_HANDLER(getitimer,	scall_getitimer),
	SCALL_HANDLER(getpgrp,		scall_getpgrp),
	SCALL_HANDLER(getpid,		scall_getpid),
	SCALL_HANDLER(getppid,		scall_getppid),
	SCALL_HANDLER(getpriority,	scall_getpriority),
	SCALL_HANDLER(gettimeofday,	scall_gettimeofday),
	SCALL_HANDLER(getuid,		scall_getuid),
	SCALL_HANDLER(kill,		do_kill),
	SCALL_HANDLER(pause,		do_pause),
	SCALL_HANDLER(ptrace,		no_sys),
	SCALL_HANDLER(reboot,		do_reboot),
	SCALL_HANDLER(setegid,		scall_setegid),
	SCALL_HANDLER(seteuid,		scall_seteuid),
	SCALL_HANDLER(setgid,		scall_setgid),
	SCALL_HANDLER(setitimer,	scall_setitimer),
	SCALL_HANDLER(setpriority,	scall_setpriority),
	SCALL_HANDLER(setsid,		scall_setsid),
	SCALL_HANDLER(setuid,		scall_setuid),
	SCALL_HANDLER(sigaction,	do_sigaction),
	SCALL_HANDLER(signal,		no_sys),	/* n/a: implemented via sigaction */
	SCALL_HANDLER(sigpending,	scall_sigpending),
	SCALL_HANDLER(sigprocmask,	scall_sigprocmask),
	SCALL_HANDLER(sigreturn,	scall_sigreturn),
	SCALL_HANDLER(sigsuspend,	scall_sigsuspend),
	SCALL_HANDLER(sprof,		do_sprofile),
	SCALL_HANDLER(stime,		scall_stime),
	SCALL_HANDLER(uname,		scall_uname),
	SCALL_HANDLER(time,		scall_time),
	SCALL_HANDLER(times,		scall_times),
	SCALL_HANDLER(wait,		do_waitpid),
	SCALL_HANDLER(waitpid,		do_waitpid),
	SCALL_HANDLER(getgroups,	scall_getgroups),
	SCALL_HANDLER(setgroups,	scall_setgroups),
};

/* This should not fail with "array size is negative": */
extern int dummy[sizeof(call_vec) <= NR_syscalls * sizeof(call_vec[0]) ? 1 : -1];
