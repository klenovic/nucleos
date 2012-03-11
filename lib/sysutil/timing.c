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
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/const.h>

#define HIGHCOUNT	0
#define LOWCOUNT	1

#define START		0
#define END		1

void util_timer_start(util_timingdata_t *timingdata, char *name)
{
	unsigned long h, l;
	int i;

	if(timingdata->names[0] == '\0') {
		for(i = 0; i < sizeof(timingdata->names) && *name; i++)
			timingdata->names[i] = *name++;
		timingdata->names[sizeof(timingdata->names)-1] = '\0';
	}

	if (timingdata->starttimes[HIGHCOUNT]) {
		panic(__FILE__, "restart timer?", NO_NUM);
		return;
	}

	read_tsc((u32_t*)&timingdata->starttimes[HIGHCOUNT],
		(u32_t*)&timingdata->starttimes[LOWCOUNT]);
}

void util_timer_end(util_timingdata_t *timingdata)
{
	unsigned long h, l, d = 0, binsize;
	int bin;

	read_tsc((u32_t*)&h, (u32_t*)&l);
	if (!timingdata->starttimes[HIGHCOUNT]) {
		panic(__FILE__, "timer stopped but not started", NO_NUM);
		return;
	}
	if (timingdata->starttimes[HIGHCOUNT] == h) {
		d = (l - timingdata->starttimes[LOWCOUNT]);
	} else if (timingdata->starttimes[HIGHCOUNT] == h-1 &&
		timingdata->starttimes[LOWCOUNT] > l) {
		d = ((ULONG_MAX - timingdata->starttimes[LOWCOUNT]) + l);
	} else {
		timingdata->misses++;
		return;
	}
	timingdata->starttimes[HIGHCOUNT] = 0;
	if (!timingdata->lock_timings_range[START] ||
		d < timingdata->lock_timings_range[START] ||
		d > timingdata->lock_timings_range[END]) {
		int t;
		if (!timingdata->lock_timings_range[START] ||
			d < timingdata->lock_timings_range[START])
			timingdata->lock_timings_range[START] = d;
		if (!timingdata->lock_timings_range[END] ||
			d > timingdata->lock_timings_range[END])
			timingdata->lock_timings_range[END] = d;
		for(t = 0; t < TIMING_POINTS; t++)
			timingdata->lock_timings[t] = 0;
		timingdata->binsize =
		 (timingdata->lock_timings_range[END] -
		 timingdata->lock_timings_range[START])/(TIMING_POINTS+1);
		if (timingdata->binsize < 1)
		  timingdata->binsize = 1;
		timingdata->resets++;
	}
	bin = (d-timingdata->lock_timings_range[START]) /
		timingdata->binsize;
	if (bin < 0 || bin >= TIMING_POINTS) {
		/* not serious, but can't happen, so shouldn't */
		panic(__FILE__, "bin out of range", bin);
	} else {
		timingdata->lock_timings[bin]++;
		timingdata->measurements++;
	}

	return;
}
