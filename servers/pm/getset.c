/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file handles the 6 system calls that get and set uids and gids.
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
 *				do_get					     *
 *===========================================================================*/
int do_get()
{
/* Handle GETUID, GETGID, GETPID, GETPGRP.
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

int sys_getegid(void)
{
	return mp->mp_effgid;
}

int sys_getgid(void)
{
	return mp->mp_realgid;
}

int sys_getpgrp(void)
{
	return mp->mp_procgrp;
}

int sys_getpid(void)
{
	return  mproc[who_p].mp_pid;
}

int sys_getppid(void)
{
	return mproc[mp->mp_parent].mp_pid;
}

int sys_getuid(void)
{
	return mp->mp_realuid;
}

/*===========================================================================*
 *				do_set					     *
 *===========================================================================*/
int do_set()
{
/* Handle SETUID, SETEUID, SETGID, SETEGID, SETSID. These calls have in common
 * that, if successful, they will be forwarded to VFS as well.
 */
  register struct mproc *rmp = mp;
  message m;
  int r;

  switch(call_nr) {
	case __NR_setuid:
	case __NR_seteuid:
		if (rmp->mp_realuid != (uid_t) m_in.usr_id && 
				rmp->mp_effuid != SUPER_USER)
			return(-EPERM);
		if(call_nr == __NR_setuid) rmp->mp_realuid = (uid_t) m_in.usr_id;
		rmp->mp_effuid = (uid_t) m_in.usr_id;

		m.m_type = PM_SETUID;
		m.PM_PROC = rmp->mp_endpoint;
		m.PM_EID = rmp->mp_effuid;
		m.PM_RID = rmp->mp_realuid;

		break;

	case __NR_setgid:
	case __NR_setegid:
		if (rmp->mp_realgid != (gid_t) m_in.grp_id && 
				rmp->mp_effuid != SUPER_USER)
			return(-EPERM);
		if(call_nr == __NR_setgid) rmp->mp_realgid = (gid_t) m_in.grp_id;
		rmp->mp_effgid = (gid_t) m_in.grp_id;

		m.m_type = PM_SETGID;
		m.PM_PROC = rmp->mp_endpoint;
		m.PM_EID = rmp->mp_effgid;
		m.PM_RID = rmp->mp_realgid;

		break;

	case __NR_setsid:
		if (rmp->mp_procgrp == rmp->mp_pid) return(-EPERM);
		rmp->mp_procgrp = rmp->mp_pid;

		m.m_type = PM_SETSID;
		m.PM_PROC = rmp->mp_endpoint;

		break;

	default:
		return(-EINVAL);
  }

  /* Send the request to FS */
  tell_fs(rmp, &m);

  /* Do not reply until FS has processed the request */
  return(SUSPEND);
}
