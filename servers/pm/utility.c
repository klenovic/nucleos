/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains some utility routines for PM.
 *
 * The entry points are:
 *   get_free_pid:	get a free process or group id
 *   no_sys:		called for invalid system call numbers
 *   find_param:	look up a boot monitor parameter
 *   find_proc:		return process pointer from pid number
 *   pm_isokendpt:	check the validity of an endpoint
 *   tell_fs:		send a request to FS on behalf of a process
 */
#include <nucleos/kernel.h>
#include "pm.h"
#include <nucleos/stat.h>
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/endpoint.h>
#include <nucleos/fcntl.h>
#include <nucleos/signal.h>		/* needed only because mproc.h needs it */
#include <nucleos/mman.h>
#include <servers/pm/mproc.h>
#include "param.h"

#include <nucleos/timer.h>
#include <nucleos/string.h>
#include <asm/kernel/const.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <kernel/proc.h>

/*===========================================================================*
 *				get_free_pid				     *
 *===========================================================================*/
pid_t get_free_pid()
{
  static pid_t next_pid = INIT_PID + 1;		/* next pid to be assigned */
  register struct mproc *rmp;			/* check process table */
  int t;					/* zero if pid still free */

  /* Find a free pid for the child and put it in the table. */
  do {
	t = 0;			
	next_pid = (next_pid < NR_PIDS ? next_pid + 1 : INIT_PID + 1);
	for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++)
		if (rmp->mp_pid == next_pid || rmp->mp_procgrp == next_pid) {
			t = 1;
			break;
		}
  } while (t);					/* 't' = 0 means pid free */
  return(next_pid);
}


/*===========================================================================*
 *				no_sys					     *
 *===========================================================================*/
int no_sys()
{
/* A system call number not implemented by PM has been requested. */
  printk("PM: in no_sys, call nr %d from %d\n", call_nr, who_e);
  return(-ENOSYS);
}

/*===========================================================================*
 *				find_proc  				     *
 *===========================================================================*/
struct mproc *find_proc(lpid)
pid_t lpid;
{
  register struct mproc *rmp;

  for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++)
	if ((rmp->mp_flags & IN_USE) && rmp->mp_pid == lpid)
		return(rmp);

  return(NIL_MPROC);
}

/*===========================================================================*
 *				pm_isokendpt			 	     *
 *===========================================================================*/
int pm_isokendpt(int endpoint, int *proc)
{
	*proc = _ENDPOINT_P(endpoint);
	if(*proc < -NR_TASKS || *proc >= NR_PROCS)
		return -EINVAL;
	if(*proc >= 0 && endpoint != mproc[*proc].mp_endpoint)
		return -EDEADSRCDST;
	if(*proc >= 0 && !(mproc[*proc].mp_flags & IN_USE))
		return -EDEADSRCDST;
	return 0;
}

/*===========================================================================*
 *				tell_fs				 	     *
 *===========================================================================*/
void tell_fs(rmp, m_ptr)
struct mproc *rmp;
kipc_msg_t *m_ptr;
{
/* Send a request to VFS, without blocking.
 */
  int r;

  if (rmp->mp_flags & FS_CALL)
	panic(__FILE__, "tell_fs: not idle", m_ptr->m_type);

  r = asynsend3(VFS_PROC_NR, m_ptr, AMF_NOREPLY);
  if (r != 0)
  	panic(__FILE__, "unable to send to FS", r);

  rmp->mp_flags |= FS_CALL;
}

int unmap_ok = 0;

int munmap(void *addrstart, size_t len)
{
	if(!unmap_ok)
		return -ENOSYS;

	/* call the definition from library */
	return __munmap(addrstart, len);
}

int munmap_text(void *addrstart, size_t len)
{
	if(!unmap_ok)
		return -ENOSYS;

	 /* call the definition from library */
	return __munmap_text(addrstart, len);

}
