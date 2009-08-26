/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
 *   find_param:	look up a boot monitor parameter
 *   get_free_pid:	get a free process or group id
 *   no_sys:		called for invalid system call numbers
 *   proc_from_pid:	return process pointer from pid number
 *   pm_isokendpt:	check the validity of an endpoint
 */
#include <nucleos/nucleos.h>
#include "pm.h"
#include <sys/stat.h>
#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/endpoint.h>
#include <fcntl.h>
#include <signal.h>		/* needed only because mproc.h needs it */
#include "mproc.h"
#include "param.h"

#include <timers.h>
#include <string.h>
#include <asm/kernel/const.h>
#include <asm/kernel/types.h>
#include <kernel/const.h>
#include <kernel/type.h>
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
  printf("PM: in no_sys, call nr %d from %d\n", call_nr, who_e);
  return(-ENOSYS);
}

/*===========================================================================*
 *				find_param				     *
 *===========================================================================*/
char *find_param(name)
const char *name;
{
  register const char *namep;
  register char *envp;

  for (envp = (char *) monitor_params; *envp != 0;) {
	for (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	if (*namep == '\0' && *envp == '=') 
		return(envp + 1);
	while (*envp++ != 0)
		;
  }
  return(NULL);
}

/*===========================================================================*
 *				proc_from_pid				     *
 *===========================================================================*/
int proc_from_pid(mp_pid)
pid_t mp_pid;
{
	int rmp;

	for (rmp = 0; rmp < NR_PROCS; rmp++)
		if (mproc[rmp].mp_pid == mp_pid)
			return rmp;

	return -1;
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

