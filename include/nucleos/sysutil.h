/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_SYSUTIL_H
#define __NUCLEOS_SYSUTIL_H

#include <nucleos/kipc.h>

/* Extra system library definitions to support device drivers and servers.
 *
 * Created:
 *	Mar 15, 2004 by Jorrit N. Herder
 *
 * Changes:
 *	May 31, 2005: added printf, kputc (relocated from syslib)
 *	May 31, 2005: added getuptime
 *	Mar 18, 2005: added tickdelay
 *	Oct 01, 2004: added env_parse, env_prefix, env_panic
 *	Jul 13, 2004: added fkey_ctl
 *	Apr 28, 2004: added report, panic 
 *	Mar 31, 2004: setup like other libraries, such as syslib
 */

/*==========================================================================* 
 * Miscellaneous helper functions.
 *==========================================================================*/ 

/* Environment parsing return values. */
#define EP_BUF_SIZE   128	/* local buffer for env value */
#define EP_UNSET	0	/* variable not set */
#define EP_OFF		1	/* var = off */
#define EP_ON		2	/* var = on (or field left blank) */
#define EP_SET		3	/* var = 1:2:3 (nonblank field) */
#define EP_EGETKENV	4	/* sys_getkenv() failed ... */

void env_setargs(int argc, char *argv[]);
int env_get_param(char *key, char *value, int max_size);
int env_prefix(char *env, char *prefix);
void env_panic(char *key);
int env_parse(char *env, char *fmt, int field, long *param, long min, long max);

#define fkey_map(fkeys, sfkeys) fkey_ctl(FKEY_MAP, (fkeys), (sfkeys))
#define fkey_unmap(fkeys, sfkeys) fkey_ctl(FKEY_UNMAP, (fkeys), (sfkeys))
#define fkey_events(fkeys, sfkeys) fkey_ctl(FKEY_EVENTS, (fkeys), (sfkeys))
int fkey_ctl(int req, int *fkeys, int *sfkeys);

int printk(const char *fmt, ...);
void kputc(int c);
void report(char *who, char *mess, int num);
void panic(char *who, char *mess, int num);
int getuptime(clock_t *ticks);
int getuptime2(clock_t *ticks, time_t *boottime);
int tickdelay(clock_t ticks);
int micro_delay_calibrate(void);
u32_t sys_hz(void);
u64_t getidle(u64_t *idle_time);
void util_stacktrace(void);
void util_nstrcat(char *str, unsigned long n);
void util_stacktrace_strcat(char *);
int micro_delay(u32_t micros);
u32_t micros_to_ticks(u32_t micros);
void ser_putc(char c);
void get_randomness(struct k_randomness *, int);

int asynsend3(endpoint_t ep, kipc_msg_t *msg, int flags);
#define asynsend(ep, msg) asynsend3(ep, msg, 0)

#define ASSERT(c) if(!(c)) { panic(__FILE__, "assert " #c " failed at line", __LINE__); }

/* timing library */
#define TIMING_CATEGORIES       20

#define TIMING_POINTS           20      /* timing resolution */
#define TIMING_CATEGORIES       20
#define TIMING_NAME             10

struct util_timingdata {
        char names[TIMING_NAME];
        unsigned long lock_timings[TIMING_POINTS]; 
        unsigned long lock_timings_range[2];
        unsigned long binsize, resets, misses, measurements;
	unsigned long starttimes[2];	/* nonzero if running */
};

typedef struct util_timingdata util_timingdata_t;

/* read_tsc() and friends. */
void read_tsc_64(u64_t *t);
void read_tsc(u32_t *hi, u32_t *lo);

#endif /* __NUCLEOS_SYSUTIL_H */
