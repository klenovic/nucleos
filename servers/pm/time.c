/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file takes care of those system calls that deal with time.
 *
 * The entry points into this file are
 *   do_time:		perform the TIME system call
 *   do_stime:		perform the STIME system call
 *   do_times:		perform the TIMES system call
 */

#include "pm.h"
#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <signal.h>
#include "mproc.h"
#include "param.h"

/*===========================================================================*
 *				do_time					     *
 *===========================================================================*/
int do_time()
{
/* Perform the time(tp) system call. This returns the time in seconds since 
 * 1.1.1970.  MINIX is an astrophysically naive system that assumes the earth 
 * rotates at a constant rate and that such things as leap seconds do not 
 * exist.
 */
  clock_t uptime;
  int s;

  if ( (s=getuptime(&uptime)) != 0) 
  	panic(__FILE__,"do_time couldn't get uptime", s);

  mp->mp_reply.reply_time = (time_t) (boottime + (uptime/system_hz));
  mp->mp_reply.reply_utime = (uptime%system_hz)*1000000/system_hz;
  return 0;
}

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
int do_stime()
{
/* Perform the stime(tp) system call. Retrieve the system's uptime (ticks 
 * since boot) and store the time in seconds at system boot in the global
 * variable 'boottime'.
 */
  clock_t uptime;
  int s;

  if (mp->mp_effuid != SUPER_USER) { 
      return(-EPERM);
  }
  if ( (s=getuptime(&uptime)) != 0) 
      panic(__FILE__,"do_stime couldn't get uptime", s);
  boottime = (long) m_in.stime - (uptime/system_hz);

  s= sys_stime(boottime);		/* Tell kernel about boottime */
  if (s != 0)
	panic(__FILE__, "pm: sys_stime failed", s);

  return 0;
}

/*===========================================================================*
 *				do_times				     *
 *===========================================================================*/
int do_times()
{
/* Perform the times(buffer) system call. */
  register struct mproc *rmp = mp;
  clock_t user_time, sys_time, uptime;
  int s;

  if ((s=sys_times(who_e, &user_time, &sys_time, &uptime)) != 0)
      panic(__FILE__,"do_times couldn't get times", s);
  rmp->mp_reply.reply_t1 = user_time;		/* user time */
  rmp->mp_reply.reply_t2 = sys_time;		/* system time */
  rmp->mp_reply.reply_t3 = rmp->mp_child_utime;	/* child user time */
  rmp->mp_reply.reply_t4 = rmp->mp_child_stime;	/* child system time */
  rmp->mp_reply.reply_t5 = uptime;		/* uptime since boot */

  return 0;
}

