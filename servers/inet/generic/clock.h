/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
clock.h

Copyright 1995 Philip Homburg
*/

#ifndef CLOCK_H
#define CLOCK_H

struct timer;

typedef void (*timer_func_t)(int fd, struct timer *timer);

typedef struct timer
{
	struct timer *tim_next;
	timer_func_t tim_func;
	int tim_ref;
	time_t tim_time;
	int tim_active;
} timer_t;

extern int clck_call_expire;	/* Call clck_expire_timer from the mainloop */

void clck_init(void);
void set_time(time_t time);
time_t get_time(void);
void reset_time(void);
/* set a timer to go off at the time specified by timeout */
void clck_timer(struct timer *timer, time_t timeout, timer_func_t func, int fd);
void clck_untimer(struct timer *timer);
void clck_expire_timers(void);

#endif /* CLOCK_H */
