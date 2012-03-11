/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <nucleos/limits.h>
#include <servers/pm/mproc.h>
#include "param.h"

int scall_getegid(void)
{
	return mp->mp_effgid;
}

int scall_getgid(void)
{
	return mp->mp_realgid;
}

int scall_getpgrp(void)
{
	return mp->mp_procgrp;
}

int scall_getpid(void)
{
	return  mproc[who_p].mp_pid;
}

int scall_getppid(void)
{
	return mproc[mp->mp_parent].mp_pid;
}

int scall_geteuid(void)
{
	return mp->mp_effuid;
}

int scall_getuid(void)
{
	return mp->mp_realuid;
}

int scall_setegid(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;

	if (rmp->mp_realgid != (gid_t) m_in.grp_id && rmp->mp_effuid != SUPER_USER)
		return -EPERM;

	rmp->mp_effgid = (gid_t) m_in.grp_id;

	m.m_type = PM_SETGID;
	m.PM_PROC = rmp->mp_endpoint;
	m.PM_EID = rmp->mp_effgid;
	m.PM_RID = rmp->mp_realgid;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}

int scall_seteuid(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;

	if (rmp->mp_realuid != (uid_t) m_in.usr_id && rmp->mp_effuid != SUPER_USER)
		return -EPERM;

	rmp->mp_effuid = (uid_t) m_in.usr_id;

	m.m_type = PM_SETUID;
	m.PM_PROC = rmp->mp_endpoint;
	m.PM_EID = rmp->mp_effuid;
	m.PM_RID = rmp->mp_realuid;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}

int scall_setgid(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;

	if (rmp->mp_realgid != (gid_t) m_in.grp_id && rmp->mp_effuid != SUPER_USER)
		return -EPERM;

	rmp->mp_realgid = (gid_t) m_in.grp_id;
	rmp->mp_effgid = (gid_t) m_in.grp_id;

	m.m_type = PM_SETGID;
	m.PM_PROC = rmp->mp_endpoint;
	m.PM_EID = rmp->mp_effgid;
	m.PM_RID = rmp->mp_realgid;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}

int scall_setsid(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;

	if (rmp->mp_procgrp == rmp->mp_pid)
		return -EPERM;

	rmp->mp_procgrp = rmp->mp_pid;

	m.m_type = PM_SETSID;
	m.PM_PROC = rmp->mp_endpoint;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}

int scall_setuid(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;

	if (rmp->mp_realuid != (uid_t) m_in.usr_id && rmp->mp_effuid != SUPER_USER)
		return -EPERM;

	rmp->mp_realuid = (uid_t) m_in.usr_id;
	rmp->mp_effuid = (uid_t) m_in.usr_id;

	m.m_type = PM_SETUID;
	m.PM_PROC = rmp->mp_endpoint;
	m.PM_EID = rmp->mp_effuid;
	m.PM_RID = rmp->mp_realuid;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}

int scall_getgroups(void)
{
	register struct mproc *rmp = mp;
	int r, i;
	int ngroups;

	kipc_msg_t m;

	ngroups = m_in.grp_no;

	if (ngroups > NGROUPS_MAX || ngroups < 0)
		return(-EINVAL);

	if (ngroups == 0) {
		r = rmp->mp_ngroups;
		return(r);
	}

	if (ngroups < rmp->mp_ngroups)
		/* Asking for less groups than available */
		return(-EINVAL);

	r = sys_datacopy(ENDPT_SELF, (vir_bytes) rmp->mp_sgroups, who_e,
			(vir_bytes) m_in.groupsp, ngroups * sizeof(gid_t));

	if (r != 0)
		return(r);

	r = rmp->mp_ngroups;

	return(r);
}

int scall_setgroups(void)
{
	register struct mproc *rmp = mp;
	kipc_msg_t m;
	int ngroups;
	int i;
	int r;

	if (rmp->mp_effuid != SUPER_USER)
		return(-EPERM);

	ngroups = m_in.grp_no;

	if (ngroups > NGROUPS_MAX || ngroups < 0)
		return(-EINVAL);

	if (m_in.groupsp == NULL)
		return(-EFAULT);

	r = sys_datacopy(who_e, (vir_bytes) m_in.groupsp, ENDPT_SELF,
			 (vir_bytes) rmp->mp_sgroups,
			 ngroups * sizeof(gid_t));

	if (r != 0)
		return(r);

	for (i = ngroups; i < NGROUPS_MAX; i++)
		rmp->mp_sgroups[i] = 0;

	rmp->mp_ngroups = ngroups;

	m.m_type = PM_SETGROUPS;
	m.PM_PROC = rmp->mp_endpoint;
	m.PM_GROUP_NO = rmp->mp_ngroups;
	m.PM_GROUP_ADDR = rmp->mp_sgroups;

	/* Send the request to FS */
	tell_fs(rmp, &m);

	/* Do not reply until FS has processed the request */
	return SUSPEND;
}
