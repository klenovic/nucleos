/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/kernel.h>
#include "sysutil.h"
#include <nucleos/timer.h>

/*===========================================================================*
 *                               tickdelay			    	     *
 *===========================================================================*/
int tickdelay(ticks)
long ticks;				/* number of ticks to wait */
{
/* This function uses the synchronous alarm to delay for a while. This works
 * even if a previous synchronous alarm was scheduled, because the remaining
 * tick of the previous alarm are returned so that it can be rescheduled.
 * Note however that a long tick_delay (longer than the remaining time of the
 * previous) alarm will also delay the previous alarm.
 */
    kipc_msg_t m, m_alarm;
    int s;

    if (ticks <= 0) return 0;		/* check for robustness */

    m.ALRM_ENDPT = ENDPT_SELF;		/* ENDPT_SELF means this process nr */
    m.ALRM_EXP_TIME = ticks;		/* request message after ticks */
    m.ALRM_ABS_TIME = 0;		/* ticks are relative to now */
    s = ktaskcall(SYSTASK, SYS_SETALARM, &m);
    if (s != 0) return(s);

    kipc_module_call(KIPC_RECEIVE, 0, CLOCK,&m_alarm);		/* await synchronous alarm */

    /* Check if we must reschedule the current alarm. */
    if (m.ALRM_TIME_LEFT > 0 && m.ALRM_TIME_LEFT != TMR_NEVER) {
    	m.ALRM_EXP_TIME = m.ALRM_TIME_LEFT - ticks;
    	if (m.ALRM_EXP_TIME <= 0) 
    		m.ALRM_EXP_TIME = 1;
    	s = ktaskcall(SYSTASK, SYS_SETALARM, &m);
    }

    return(s);
}





