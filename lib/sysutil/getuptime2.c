/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"

/*===========================================================================*
 *                               getuptime2			    	     *
 *===========================================================================*/
int getuptime2(ticks, boottime)
clock_t *ticks;				/* uptime in ticks */
time_t *boottime;
{
    message m;
    int s;

    m.m_type = SYS_TIMES;		/* request time information */
    m.T_ENDPT = NONE;			/* ignore process times */
    s = _taskcall(SYSTASK, SYS_TIMES, &m);
    *ticks = m.T_BOOT_TICKS;
    *boottime = m.T_BOOTTIME;
    return(s);
}





