/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <nucleos/u64.h>
#include <nucleos/config.h>
#include <nucleos/const.h>

#include "sysutil.h"

static u32_t Hz;

u32_t
sys_hz(void)
{
	if(Hz <= 0) {
		int r;
		/* Get HZ. */
		if((r=sys_getinfo(GET_HZ, &Hz, sizeof(Hz), 0, 0)) != OK) {
			Hz = DEFAULT_HZ;
			printf("sys_hz: %d: reverting to HZ = %d\n", r, Hz);
		}
	}

	return Hz;
}

u32_t
micros_to_ticks(u32_t micros)
{
        u32_t ticks;

        ticks = div64u(mul64u(micros, sys_hz()), 1000000);
        if(ticks < 1) ticks = 1;

        return ticks;
}


