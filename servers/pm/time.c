/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/signal.h>
#include <nucleos/time.h>
#include <nucleos/times.h>
#include <servers/pm/mproc.h>
#include "param.h"

#define p_time		m_data4

int scall_time(void)
{
	/* Perform the time(tp) system call. This returns the time in seconds since 
	 * 1.1.1970.  MINIX is an astrophysically naive system that assumes the earth 
	 * rotates at a constant rate and that such things as leap seconds do not 
	 * exist.
	 */
	clock_t uptime, boottime;
	time_t t;
	int err;
	int s;

	if ((s = getuptime2(&uptime, &boottime)) != 0) 
		panic(__FILE__,"do_time couldn't get uptime", s);

	t = (time_t)(boottime + (uptime/system_hz));

	/* if argument is not NULL then make a copy there too */
	if (m_in.p_time != 0) {
		err = sys_vircopy(ENDPT_SELF, D, (vir_bytes)&t, mp->mp_endpoint, D, (vir_bytes)m_in.p_time,
				  sizeof(time_t));

		if (err != 0)
			return -EFAULT;
	}

	return t;
}

#define pstime		m_data6

int scall_stime(void)
{
	/* Perform the stime(tp) system call. Retrieve the system's uptime (ticks 
	 * since boot) and pass the new time in seconds at system boot to the kernel.
	 */
	clock_t uptime, boottime;
	time_t t;
	int err;
	int s;

	if (mp->mp_effuid != SUPER_USER) { 
		return -EPERM;
	}

	if ((s=getuptime(&uptime)) != 0)
		panic(__FILE__,"scall_stime couldn't get uptime", s);

	err = sys_vircopy(mp->mp_endpoint, D, (vir_bytes)m_in.pstime, ENDPT_SELF, D, (vir_bytes)&t, sizeof(time_t));

	if (err != 0)
		return -EFAULT;

	boottime = (long)t - (uptime/system_hz);

	s = sys_stime(boottime);		/* Tell kernel about boottime */

	if (s != 0)
		panic(__FILE__, "pm: sys_stime failed", s);

	return 0;
}

#define p_timesbuf	m_data4

int scall_times(void)
{
	/* Perform the times(buffer) system call. */
	register struct mproc *rmp = mp;
	clock_t user_time, sys_time, uptime;
	struct tms buf;
	int err;
	int s;

	if ((s = sys_times(who_e, &user_time, &sys_time, &uptime, NULL)) != 0)
		panic(__FILE__,"scall_times couldn't get times", s);

	buf.tms_utime = user_time;		/* user time */
	buf.tms_stime = sys_time;		/* system time */
	buf.tms_cutime = rmp->mp_child_utime;	/* child user time */
	buf.tms_cstime = rmp->mp_child_stime;	/* child system time */

	err = sys_vircopy(ENDPT_SELF, D, (vir_bytes)&buf, mp->mp_endpoint, D, (vir_bytes)m_in.p_timesbuf,
			  sizeof(struct tms));

	if (err != 0)
		return -EFAULT;

	return uptime;
}

#define ptimeval	m_data6

int scall_gettimeofday(void)
{
	/* Perform the time(tp) system call. This returns the time in seconds since
	 * 1.1.1970.  Nucleos is an astrophysically naive system that assumes the earth
	 * rotates at a constant rate and that such things as leap seconds do not
	 * exist.
	 */
	clock_t uptime, boottime;
	struct timeval tv;
	int ret;
	int s;

	if ((s = getuptime2(&uptime, &boottime)) != 0)
		panic(__FILE__,"do_gettimeofday couldn't get uptime", s);

	tv.tv_sec = (boottime + (uptime/system_hz));
	tv.tv_usec = (uptime%system_hz)*1000000/system_hz;

	ret = sys_vircopy(ENDPT_SELF, D, (vir_bytes)&tv, mp->mp_endpoint, D, (vir_bytes)m_in.ptimeval,
			  sizeof(struct timeval));

	return (ret < 0) ? -EFAULT : 0;
}
