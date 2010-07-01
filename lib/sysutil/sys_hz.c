/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <stdio.h>
#include <nucleos/time.h>
#include <nucleos/time.h>
#include <nucleos/times.h>
#include <nucleos/types.h>
#include <nucleos/u64.h>
#include <nucleos/const.h>
#include <nucleos/param.h>

#include "sysutil.h"

static u32_t hz;

u32_t
sys_hz(void)
{
	if(hz <= 0) {
		int r;
		/* Get HZ. */
		if((r=sys_getinfo(GET_HZ, &hz, sizeof(hz), 0, 0)) != 0) {
			hz = HZ;
			printf("sys_hz: %d: reverting to HZ = %d\n", r, hz);
		}
	}

	return hz;
}

u32_t
micros_to_ticks(u32_t micros)
{
        u32_t ticks;

        ticks = div64u(mul64u(micros, sys_hz()), 1000000);
        if(ticks < 1) ticks = 1;

        return ticks;
}


