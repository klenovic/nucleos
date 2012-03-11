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
#include "timers.h"

/*===========================================================================*
 *				tmrs_exptimers				     *
 *===========================================================================*/
void tmrs_exptimers(tmrs, now, new_head)
timer_t **tmrs;				/* pointer to timers queue */
clock_t now;				/* current time */
clock_t *new_head;
{
/* Use the current time to check the timers queue list for expired timers. 
 * Run the watchdog functions for all expired timers and deactivate them.
 * The caller is responsible for scheduling a new alarm if needed.
 */
  timer_t *tp;

  while ((tp = *tmrs) != NULL && tp->tmr_exp_time <= now) {
	*tmrs = tp->tmr_next;
	tp->tmr_exp_time = TMR_NEVER;
	(*tp->tmr_func)(tp);
  }

  if(new_head) {
  	if(*tmrs)
  		*new_head = (*tmrs)->tmr_exp_time;
  	else
  		*new_head = 0;
  }
}


