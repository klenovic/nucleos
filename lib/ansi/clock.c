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
 * clock - determine the processor time used
 */
#include <nucleos/time.h>
#include <nucleos/times.h>

clock_t clock(void)
{
	struct tms tms;

	times(&tms);
	return tms.tms_utime;
}
