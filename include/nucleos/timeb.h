#ifndef _NUCLEOS_TIMEB_H
#define _NUCLEOS_TIMEB_H

#include <nucleos/time.h>

struct timeb
{
	time_t time;                /* Seconds since epoch, as from `time'.  */
	unsigned short int millitm; /* Additional milliseconds.  */
	short int timezone;         /* Minutes west of GMT.  */
	short int dstflag;          /* Nonzero if Daylight Savings Time used.  */
};

/* Fill in TIMEBUF with information about the current time.  */

int ftime(struct timeb *__timebuf);

#endif /* _NUCLEOS_TIMEB_H */
