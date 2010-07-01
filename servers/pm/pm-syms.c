/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "pm.h"
#include <nucleos/unistd.h>
#include <nucleos/signal.h>
#include <servers/pm/mproc.h>
#include "param.h"

/* Global variables. */
struct mproc *mp;	/* ptr to 'mproc' slot of current process */
int procs_in_use;	/* how many processes are marked as IN_USE */
char monitor_params[128*sizeof(char *)];	/* boot monitor parameters */
struct kinfo kinfo;				/* kernel information */

/* The parameters of the call are kept here. */
kipc_msg_t m_in;		/* the incoming message itself is kept here. */
int who_p, who_e;	/* caller's proc number, endpoint */
int call_nr;		/* system call number */
sigset_t core_sset;	/* which signals cause core images */
sigset_t ign_sset;	/* which signals are by default ignored */
time_t boottime;	/* time when the system was booted (for reporting to FS_PROC_NR) */
u32_t system_hz;	/* System clock frequency. */
int report_reboot;	/* During reboot to report to FS_PROC_NR that we are rebooting. */
int abort_flag;
char monitor_code[256];
