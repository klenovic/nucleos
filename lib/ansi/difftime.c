/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * difftime - compute the difference between two calendar times
 */
#include	<time.h>

double
difftime(time_t time1, time_t time0)
{
	/* be careful: time_t may be unsigned */
	if ((time_t)-1 > 0 && time0 > time1) {
		return - (double) (time0 - time1);
	} else {
		return (double)(time1 - time0);
	}
}
