/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __PM_GLO_H
#define __PM_GLO_H

/* Global variables. */
extern struct mproc *mp;	/* ptr to 'mproc' slot of current process */
extern int procs_in_use;	/* how many processes are marked as IN_USE */
extern char monitor_params[128*sizeof(char *)];	/* boot monitor parameters */
extern struct kinfo kinfo;	/* kernel information */

/* Misc.c */
extern struct utsname uts_val;	/* uname info */

/* The parameters of the call are kept here. */
extern kipc_msg_t m_in;		/* the incoming message itself is kept here. */
extern int who_p, who_e;	/* caller's proc number, endpoint */
extern int call_nr;		/* system call number */

extern int (*call_vec[])(void);	/* system call handlers */
extern sigset_t core_sset;	/* which signals cause core images */
extern sigset_t ign_sset;	/* which signals are by default ignored */

extern u32_t system_hz;		/* System clock frequency. */
extern int abort_flag;
extern char monitor_code[256];
		
#endif /* __PM_GLO_H */
