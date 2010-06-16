/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file handles the EXEC system call.  It performs the work as follows:
 *    - see if the permissions allow the file to be executed
 *    - read the header and extract the sizes
 *    - fetch the initial args and environment from the user space
 *    - allocate the memory for the new process
 *    - copy the initial stack from PM to the process
 *    - read in the text and data segments and copy to the process
 *    - take care of setuid and setgid bits
 *    - fix up 'mproc' table
 *    - tell kernel about EXEC
 *    - save offset to initial argc (for ps)
 *
 * The entry points into this file are:
 *   do_exec:	 perform the EXEC system call
 *   exec_newmem: allocate new memory map for a process that tries to exec
 *   do_execrestart: finish the special exec call for RS
 *   exec_restart: finish a regular exec call
 *   find_share: find a process whose text segment can be shared
 */

#include "pm.h"
#include <nucleos/stat.h>
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>
#include <nucleos/com.h>
#include <nucleos/vm.h>
#include <nucleos/a.out.h>
#include <nucleos/signal.h>
#include <nucleos/string.h>
#include <nucleos/ptrace.h>
#include <servers/pm/mproc.h>
#include "param.h"

#define ESCRIPT	(-2000)	/* Returned by read_header for a #! script. */
#define PTRSIZE	sizeof(char *) /* Size of pointers in argv[] and envp[]. */

/*===========================================================================*
 *				do_exec					     *
 *===========================================================================*/
int do_exec()
{
	kipc_msg_t m;
	int r;

	m.m_type = PM_EXEC;
	m.PM_PROC = mp->mp_endpoint;
	m.PM_PATH = m_in.exec_name;
	m.PM_PATH_LEN = m_in.exec_len;
	m.PM_FRAME = m_in.stack_ptr;
	m.PM_FRAME_LEN = m_in.stack_bytes;

	tell_fs(mp, &m);

	/* Do not reply */
	return SUSPEND;
}

/*===========================================================================*
 *				exec_newmem				     *
 *===========================================================================*/
int exec_newmem()
{
	int proc_e, proc_n, allow_setuid;
	char *ptr;
	struct mproc *rmp;
	struct exec_newmem args;
	int r, flags;
	char *stack_top;

	if (who_e != FS_PROC_NR && who_e != RS_PROC_NR)
		return -EPERM;

	proc_e = m_in.EXC_NM_PROC;

	if (pm_isokendpt(proc_e, &proc_n) != 0) {
		panic(__FILE__, "exec_newmem: got bad endpoint",
			proc_e);
	}

	rmp = &mproc[proc_n];
	ptr = m_in.EXC_NM_PTR;

	/* copy parameters from process (sent by VFS) */
	r = sys_datacopy(who_e, (vir_bytes)ptr,
		ENDPT_SELF, (vir_bytes)&args, sizeof(args));

	if (r != 0)
		panic(__FILE__, "exec_newmem: sys_datacopy failed", r);

	if ((r = vm_exec_newmem(proc_e, &args, sizeof(args), &stack_top, &flags)) == 0) {
		allow_setuid = 0; /* Do not allow setuid execution */

		if (rmp->mp_tracer == NO_TRACER) {
			/* Okay, setuid execution is allowed */
			allow_setuid = 1;
			rmp->mp_effuid = args.new_uid;
			rmp->mp_effgid = args.new_gid;
		}

		/* System will save command line for debugging, ps(1) output, etc. */
		strncpy(rmp->mp_name, args.progname, PROC_NAME_LEN-1);
		rmp->mp_name[PROC_NAME_LEN-1] = '\0';

		/* Save offset to initial argc (for ps) */
		rmp->mp_procargs = (vir_bytes) stack_top - args.args_bytes;

		/* Kill process if something goes wrong after this point. */
		rmp->mp_flags |= PARTIAL_EXEC;

		mp->mp_reply.reply_res2 = (vir_bytes) stack_top;
		mp->mp_reply.reply_res3 = flags;
		if (allow_setuid)
			mp->mp_reply.reply_res3 |= EXC_NM_RF_ALLOW_SETUID;

		rmp->entry_point = args.entry_point;
	} else {
		printf("PM: newmem failed for %s\n", args.progname);
	}

	return r;
}

/*===========================================================================*
 *				do_execrestart				     *
 *===========================================================================*/
int do_execrestart()
{
	int proc_e, proc_n, result;
	struct mproc *rmp;

	if (who_e != RS_PROC_NR)
		return -EPERM;

	proc_e= m_in.EXC_RS_PROC;

	if (pm_isokendpt(proc_e, &proc_n) != 0) {
		panic(__FILE__, "do_execrestart: got bad endpoint",
			proc_e);
	}

	rmp= &mproc[proc_n];
	result= m_in.EXC_RS_RESULT;

	exec_restart(rmp, result);

	return 0;
}

/*===========================================================================*
 *				exec_restart				     *
 *===========================================================================*/
void exec_restart(rmp, result)
struct mproc *rmp;
int result;
{
	int r, sn;
	char *new_sp;

	if (result != 0)
	{
		if (rmp->mp_flags & PARTIAL_EXEC)
		{
			/* Use SIGILL signal that something went wrong */
			rmp->mp_sigstatus = SIGILL;
			exit_proc(rmp, 0, FALSE /*dump_core*/);
			return;
		}

		setreply(rmp-mproc, result);

		return;
	}

	rmp->mp_flags &= ~PARTIAL_EXEC;

	/* Fix 'mproc' fields, tell kernel that exec is done, reset caught
	 * sigs.
	 */
	for (sn = 1; sn < _NSIG; sn++) {
		if (sigismember(&rmp->mp_catch, sn)) {
			sigdelset(&rmp->mp_catch, sn);
			rmp->mp_sigact[sn].sa_handler = SIG_DFL;
			sigemptyset(&rmp->mp_sigact[sn].sa_mask);
		}
	}

	/* Cause a signal if this process is traced.
	 * Do this before making the process runnable again!
	 */
	if (rmp->mp_tracer != NO_TRACER && !(rmp->mp_trace_flags & TO_NOEXEC))
	{
 		sn = (rmp->mp_trace_flags & TO_ALTEXEC) ? SIGSTOP : SIGTRAP;
		check_sig(rmp->mp_pid, sn);
	}

	new_sp = (char *)rmp->mp_procargs;

	r = sys_exec(rmp->mp_endpoint, new_sp, rmp->mp_name, rmp->entry_point);

	if (r != 0) panic(__FILE__, "sys_exec failed", r);

	return;
}
