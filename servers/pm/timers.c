/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* PM watchdog timer management. These functions in this file provide
 * a convenient interface to the timers library that manages a list of
 * watchdog timers. All details of scheduling an alarm at the CLOCK task 
 * are hidden behind this interface.
 * Only system processes are allowed to set an alarm timer at the kernel. 
 * Therefore, the PM maintains a local list of timers for user processes
 * that requested an alarm signal. 
 * 
 * The entry points into this file are:
 *   pm_set_timer:      reset and existing or set a new watchdog timer
 *   pm_expire_timers:  check for expired timers and run watchdog functions
 *   pm_cancel_timer:   remove a time from the list of timers
 *
 */

#include "pm.h"

#include <timers.h>
#include <nucleos/syslib.h>
#include <nucleos/com.h>

static timer_t *pm_timers = NULL;
static int pm_expiring = 0;

/*===========================================================================*
 *				pm_set_timer				     *
 *===========================================================================*/
void pm_set_timer(timer_t *tp, int ticks, tmr_func_t watchdog, int arg)
{
	int r;
	clock_t now, prev_time = 0, next_time;

	if ((r = getuptime(&now)) != 0)
		panic(__FILE__, "PM couldn't get uptime", NO_NUM);

	/* Set timer argument and add timer to the list. */
	tmr_arg(tp)->ta_int = arg;
	prev_time = tmrs_settimer(&pm_timers,tp,now+ticks,watchdog,&next_time);

	/* Reschedule our synchronous alarm if necessary. */
	if (pm_expiring == 0 && (! prev_time || prev_time > next_time)) {
		if (sys_setalarm(next_time, 1) != 0)
			panic(__FILE__, "PM set timer couldn't set alarm.", NO_NUM);
	}

	return;
}

/*===========================================================================*
 *				pm_expire_timers			     *
 *===========================================================================*/
void pm_expire_timers(clock_t now)
{
	clock_t next_time;

	/* Check for expired timers. Use a global variable to indicate that
	 * watchdog functions are called, so that sys_setalarm() isn't called
	 * more often than necessary when pm_set_timer or pm_cancel_timer are
	 * called from these watchdog functions. */
	pm_expiring = 1;
	tmrs_exptimers(&pm_timers, now, &next_time);
	pm_expiring = 0;

	/* Reschedule an alarm if necessary. */
	if (next_time > 0) {
		if (sys_setalarm(next_time, 1) != 0)
			panic(__FILE__, "PM expire timer couldn't set alarm.", NO_NUM);
	}
}

/*===========================================================================*
 *				pm_cancel_timer				     *
 *===========================================================================*/
void pm_cancel_timer(timer_t *tp)
{
	clock_t next_time, prev_time;
	prev_time = tmrs_clrtimer(&pm_timers, tp, &next_time);

	/* If the earliest timer has been removed, we have to set the alarm to  
     * the next timer, or cancel the alarm altogether if the last timer has 
     * been cancelled (next_time will be 0 then).
	 */
	if (pm_expiring == 0 && (prev_time < next_time || ! next_time)) {
		if (sys_setalarm(next_time, 1) != 0)
			panic(__FILE__, "PM expire timer couldn't set alarm.", NO_NUM);
	}
}
