/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file handles the 4 system calls that get and set uids and gids.
 * It also handles getpid(), setsid(), and getpgrp().  The code for each
 * one is so tiny that it hardly seemed worthwhile to make each a separate
 * function.
 */

#include "pm.h"
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>
#include <nucleos/signal.h>
#include "mproc.h"
#include "param.h"

/*===========================================================================*
 *				do_getset				     *
 *===========================================================================*/
int do_getset()
{
/* Handle GETUID, GETGID, GETPID, GETPGRP, SETUID, SETGID, SETSID.  The four
 * GETs and SETSID return their primary results in 'r'.  GETUID, GETGID, and
 * GETPID also return secondary results (the effective IDs, or the parent
 * process ID) in 'reply_res2', which is returned to the user.
 */

  register struct mproc *rmp = mp;
  int r, proc;

  switch(call_nr) {
	case __NR_getuid:
		r = rmp->mp_realuid;
		rmp->mp_reply.reply_res2 = rmp->mp_effuid;
		break;

	case __NR_getgid:
		r = rmp->mp_realgid;
		rmp->mp_reply.reply_res2 = rmp->mp_effgid;
		break;

	case __NR_getpid:
		r = mproc[who_p].mp_pid;
		rmp->mp_reply.reply_res2 = mproc[rmp->mp_parent].mp_pid;
		if(pm_isokendpt(m_in.endpt, &proc) == 0 && proc >= 0)
			rmp->mp_reply.reply_res3 = mproc[proc].mp_pid;
		break;

	case __NR_seteuid:
	case __NR_setuid:
		if (rmp->mp_realuid != (uid_t) m_in.usr_id && 
				rmp->mp_effuid != SUPER_USER)
			return(-EPERM);
		if(call_nr == __NR_setuid) rmp->mp_realuid = (uid_t) m_in.usr_id;
		rmp->mp_effuid = (uid_t) m_in.usr_id;

		if (rmp->mp_fs_call != PM_IDLE)
		{
			panic(__FILE__, "do_getset: not idle",
				rmp->mp_fs_call);
		}
		rmp->mp_fs_call= PM_SETUID;
		r= kipc_notify(FS_PROC_NR);
		if (r != 0)
			panic(__FILE__, "do_getset: unable to notify FS_PROC_NR", r);
		
		/* Do not reply until FS_PROC_NR is ready to process the setuid
		 * request
		 */
		r= SUSPEND;
		break;

	case __NR_setegid:
	case __NR_setgid:
		if (rmp->mp_realgid != (gid_t) m_in.grp_id && 
				rmp->mp_effuid != SUPER_USER)
			return(-EPERM);
		if(call_nr == __NR_setgid) rmp->mp_realgid = (gid_t) m_in.grp_id;
		rmp->mp_effgid = (gid_t) m_in.grp_id;

		if (rmp->mp_fs_call != PM_IDLE)
		{
			panic(__FILE__, "do_getset: not idle",
				rmp->mp_fs_call);
		}
		rmp->mp_fs_call= PM_SETGID;
		r= kipc_notify(FS_PROC_NR);
		if (r != 0)
			panic(__FILE__, "do_getset: unable to notify FS_PROC_NR", r);

		/* Do not reply until FS_PROC_NR is ready to process the setgid
		 * request
		 */
		r= SUSPEND;
		break;

	case __NR_setsid:
		if (rmp->mp_procgrp == rmp->mp_pid) return(-EPERM);
		rmp->mp_procgrp = rmp->mp_pid;

		if (rmp->mp_fs_call != PM_IDLE)
		{
			panic(__FILE__, "do_getset: not idle",
				rmp->mp_fs_call);
		}
		rmp->mp_fs_call= PM_SETSID;
		r= kipc_notify(FS_PROC_NR);
		if (r != 0)
			panic(__FILE__, "do_getset: unable to notify FS_PROC_NR", r);

		/* Do not reply until FS_PROC_NR is ready to process the setsid
		 * request
		 */
		r= SUSPEND;
		break;

	case __NR_getpgrp:
		r = rmp->mp_procgrp;
		break;

	default:
		r = -EINVAL;
		break;	
  }
  return(r);
}
