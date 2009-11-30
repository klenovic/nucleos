/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_TIME_H
#define __NUCLEOS_TIME_H

#include <nucleos/types.h>


/* Open Group Base Specifications Issue 6 (not complete) */
struct timeval
{
	long /*time_t*/ tv_sec;
	long /*useconds_t*/ tv_usec;
};

struct timezone {
	int     tz_minuteswest; /* minutes west of Greenwich */
	int     tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *__restrict tp, void *__restrict tzp);

/* Compatibility with other Unix systems */
int settimeofday(const struct timeval *tp, const void *tzp);

/* setitimer/getitimer interface */
struct itimerval
{
	struct timeval it_interval;
	struct timeval it_value;
};

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1	/* Not implemented */
#define ITIMER_PROF 2		/* Not implemented */

int getitimer(int which, struct itimerval *value);
int setitimer(int which, const struct itimerval *__restrict value,
	      struct itimerval *__restrict ovalue);

#include <nucleos/select.h>

#define NFDBITS			__NFDBITS

#define FD_SETSIZE		__FD_SETSIZE
#define FD_ZERO(fdsetp)		__FD_ZERO(fdsetp)
#define FD_SET(fd,fdsetp)	__FD_SET(fd,fdsetp)
#define FD_CLR(fd,fdsetp)	__FD_CLR(fd,fdsetp)
#define FD_ISSET(fd,fdsetp)	__FD_ISSET(fd,fdsetp)

#if defined(__KERNEL__) || defined(__UKERNEL__)

struct tm {
	int tm_sec;	/* seconds after the minute [0, 59] */
	int tm_min;	/* minutes after the hour [0, 59] */
	int tm_hour;	/* hours since midnight [0, 23] */
	int tm_mday;	/* day of the month [1, 31] */
	int tm_mon;	/* months since January [0, 11] */
	int tm_year;	/* years since 1900 */
	int tm_wday;	/* days since Sunday [0, 6] */
	int tm_yday;	/* days since January 1 [0, 365] */
	int tm_isdst;	/* Daylight Saving Time flag */
};

extern char *tzname[];

clock_t clock(void);
double difftime(time_t _time1, time_t _time0);
time_t mktime(struct tm *_timeptr);
time_t time(__kernel_time_t *_timeptr);
char *asctime(const struct tm *_timeptr);
char *ctime(const time_t *_timer);
struct tm *gmtime(const time_t *_timer);
struct tm *localtime(const time_t *_timer);
size_t strftime(char *_s, size_t _max, const char *_fmt, const struct tm *_timep);

void tzset(void);

int stime(time_t *_top);

extern long timezone;

struct timespec
{
	time_t tv_sec;
	long tv_nsec;
};

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */



#endif /* __NUCLEOS_TIME_H */
