/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"

/*===========================================================================*
 *                               getuptime			    	     *
 *===========================================================================*/
int getuptime(ticks)
clock_t *ticks;				/* uptime in ticks */
{
    kipc_msg_t m;
    int s;

    m.m_type = SYS_TIMES;		/* request time information */
    m.T_ENDPT = ENDPT_NONE;			/* ignore process times */
    s = ktaskcall(SYSTASK, SYS_TIMES, &m);
    *ticks = m.T_BOOT_TICKS;
    return(s);
}





