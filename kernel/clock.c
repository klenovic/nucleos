/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the clock task, which handles time related functions.
 * Important events that are handled by the CLOCK include setting and 
 * monitoring alarm timers and deciding when to (re)schedule processes. 
 * The CLOCK offers a direct interface to kernel processes. System services 
 * can access its services through system calls, such as sys_setalarm(). The
 * CLOCK task thus is hidden from the outside world.  
 *
 * Changes:
 *   Aug 18, 2006   removed direct hardware access etc, MinixPPC (Ingmar Alting)
 *   Oct 08, 2005   reordering and comment editing (A. S. Woodhull)
 *   Mar 18, 2004   clock interface moved to SYSTEM task (Jorrit N. Herder) 
 *   Sep 30, 2004   source code documentation updated  (Jorrit N. Herder)
 *   Sep 24, 2004   redesigned alarm timers  (Jorrit N. Herder)
 *
 * Clock task is notified by the clock's interrupt handler when a timer
 * has expired.
 *
 * In addition to the main clock_task() entry point, which starts the main 
 * loop, there are several other minor entry points:
 *   clock_stop:	called just before MINIX shutdown
 *   get_uptime:	get realtime since boot in clock ticks
 *   set_timer:		set a watchdog timer (+)
 *   reset_timer:	reset a watchdog timer (+)
 *   read_clock:	read the counter of channel 0 of the 8253A timer
 *
 * (+) The CLOCK task keeps tracks of watchdog timers for the entire kernel.
 * It is crucial that watchdog functions not block, or the CLOCK task may
 * be blocked. Do not send() a message when the receiver is not expecting it.
 * Instead, notify(), which always returns, should be used.
 */
#include <nucleos/signal.h>
#include <nucleos/com.h>
#include <nucleos/endpoint.h>
#include <nucleos/portio.h>
#include <kernel/clock.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>

/* The CLOCK's timers queue. The functions in <nucleos/timer.h> operate on this.
 * Each system process possesses a single synchronous alarm timer. If other
 * kernel parts want to use additional timers, they must declare their own
 * persistent (static) timer structure, which can be passed to the clock
 * via (re)set_timer().
 * When a timer expires its watchdog function is run by the CLOCK task.
 */
static timer_t *clock_timers;	/* queue of CLOCK timers */
static clock_t next_timeout;	/* realtime that next timer expires */

/* The time is incremented by the interrupt handler on each clock tick. */
static clock_t realtime = 0;		/* real time clock */

static void init_clock(void)
{
	/* Set a watchdog timer to periodically balance the scheduling queues.
	   Side-effect sets new timer */

	balance_queues(NULL);
}

static void load_update(void)
{
	u16_t slot;
	int enqueued = 0, q;
	struct proc *p;

	/* Load average data is stored as a list of numbers in a circular
	 * buffer. Each slot accumulates _LOAD_UNIT_SECS of samples of
	 * the number of runnable processes. Computations can then
	 * be made of the load average over variable periods, in the
	 * user library (see getloadavg(3)).
	 */
	slot = (realtime / system_hz / _LOAD_UNIT_SECS) % _LOAD_HISTORY;
	if(slot != kloadinfo.proc_last_slot) {
		kloadinfo.proc_load_history[slot] = 0;
		kloadinfo.proc_last_slot = slot;
	}

	/* Cumulation. How many processes are ready now? */
	for(q = 0; q < NR_SCHED_QUEUES; q++)
		for(p = rdy_head[q]; p != NIL_PROC; p = p->p_nextready)
			enqueued++;

	kloadinfo.proc_load_history[slot] += enqueued;

	/* Up-to-dateness. */
	kloadinfo.last_clock = realtime;
}

/*===========================================================================*
 *				clock_task				     *
 *===========================================================================*/
void clock_task()
{
/* Main program of clock task. If the call is not HARD_INT it is an error. */
	kipc_msg_t m;	/* message buffer for both input and output */
	int result;	/* result returned by the handler */

	init_clock();    /* initialize clock task */

	/* Main loop of the clock task.  Get work, process it. Never reply. */
	while(1) {
		/* Go get a message. */
		result = kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, &m);

		if(result != 0)
			kernel_panic("kipc_module_call type KIPC_RECEIVE failed", result);

		/* Handle the request. Only clock ticks are expected. */
		if (is_notify(m.m_type)) {
			switch (_ENDPOINT_P(m.m_source)) {
			case HARDWARE:
				tmrs_exptimers(&clock_timers, realtime, NULL);
				next_timeout = (clock_timers == NULL) ?
				TMR_NEVER : clock_timers->tmr_exp_time;
				break;
			default:	/* illegal request type */
				printk("CLOCK: illegal notify %d from %d.\n", m.m_type, m.m_source);
			}
		}
		else {
			/* illegal request type */
			printk("CLOCK: illegal request %d from %d.\n", m.m_type, m.m_source);
		}
	}
}

/*
 * The boot processor timer interrupt handler. In addition to non-boot cpus it
 * keeps real time and notifies the clock task if need be
 */
int bsp_timer_int_handler(void)
{
	unsigned ticks;

	IDLE_STOP;

	if(kernel_in_panic)
		return 0;

	/* Get number of ticks and update realtime. */
	ticks = lost_ticks + 1;
	lost_ticks = 0;
	realtime += ticks;

	ap_timer_int_handler();

	/* if a timer expired, notify the clock task */
	if ((next_timeout <= realtime)) {
		mini_notify(proc_addr(HARDWARE), CLOCK); /* send notification */
	}

	if (do_serial_debug)
		do_ser_debug();

	return(1);					/* reenable interrupts */
}

clock_t get_uptime(void)
{
	/* Get and return the current clock uptime in ticks. */
	return(realtime);
}

void set_timer(tp, exp_time, watchdog)
struct timer *tp;		/* pointer to timer structure */
clock_t exp_time;		/* expiration realtime */
tmr_func_t watchdog;		/* watchdog to be called */
{
	/* Insert the new timer in the active timers list. Always update the 
	 * next timeout time by setting it to the front of the active list.
	 */
	tmrs_settimer(&clock_timers, tp, exp_time, watchdog, NULL);
	next_timeout = clock_timers->tmr_exp_time;
}

void reset_timer(struct timer *tp)
{
	/* The timer pointed to by 'tp' is no longer needed. Remove it from both the
	 * active and expired lists. Always update the next timeout time by setting
	 * it to the front of the active list.
	 */
	tmrs_clrtimer(&clock_timers, tp, NULL);
	next_timeout = (clock_timers == NULL) ? 
	TMR_NEVER : clock_timers->tmr_exp_time;
}

/*
 * Timer interupt handler. This is the only think executed on non boot
 * processors. It is called by bsp_timer_int_handler() on the boot processor
 */
int ap_timer_int_handler(void)
{

	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	unsigned ticks = 1;
	int expired = 0;
	struct proc * p, * billp;

	IDLE_STOP;

	/* Update user and system accounting times. Charge the current process
	 * for user time. If the current process is not billable, that is, if a
	 * non-user process is running, charge the billable process for system
	 * time as well.  Thus the unbillable process' user time is the billable
	 * user's system time.
	 */

	/* FIXME prepared for get_cpu_local_var() */
	p = proc_ptr;
	billp = bill_ptr;

	p->p_user_time += ticks;
	if (priv(p)->s_flags & PREEMPTIBLE) {
		p->p_ticks_left -= ticks;
	}
	if (! (priv(p)->s_flags & BILLABLE)) {
		billp->p_sys_time += ticks;
		billp->p_ticks_left -= ticks;
	}

	/* Decrement virtual timers, if applicable. We decrement both the
	 * virtual and the profile timer of the current process, and if the
	 * current process is not billable, the timer of the billed process as
	 * well.  If any of the timers expire, do_clocktick() will send out
	 * signals.
	 */
	if ((p->p_misc_flags & MF_VIRT_TIMER) && (p->p_virt_left -= ticks) <= 0)
		expired = 1;

	if ((p->p_misc_flags & MF_PROF_TIMER) && (p->p_prof_left -= ticks) <= 0)
		expired = 1;

	if (!(priv(p)->s_flags & BILLABLE) && (billp->p_misc_flags & MF_PROF_TIMER) &&
	    (billp->p_prof_left -= ticks) <= 0)
		expired = 1;

	/*
	 * Check if a process-virtual timer expired. Check current process, but
	 * also bill_ptr - one process's user time is another's system time, and
	 * the profile timer decreases for both!
	 */
	vtimer_check(p);

	if (p != billp)
		vtimer_check(billp);

	/* Update load average. */
	load_update();

	/* check if the process is still runnable after checking the vtimer */
	if (p->p_rts_flags == 0 && p->p_ticks_left <= 0 &&
			priv(p)->s_flags & PREEMPTIBLE) {
		/* this dequeues the process */
		RTS_SET(p, RTS_NO_QUANTUM);
	}

	return 1;
}

int boot_cpu_init_timer(unsigned freq)
{
	if (arch_init_local_timer(freq))
		return -1;

	if (arch_register_local_timer_handler((irq_handler_t) bsp_timer_int_handler))
		return -1;

	return 0;
}
