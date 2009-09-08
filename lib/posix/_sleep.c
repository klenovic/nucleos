/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sleep() - Sleep for a number of seconds.	Author: Erik van der Kouwe
 *								25 July 2009
 * (Avoids interfering with alarm/setitimer by using select, like usleep)
 */
#include <nucleos/lib.h>
#include <nucleos/signal.h>
#include <nucleos/unistd.h>
#include <nucleos/errno.h>
#include <nucleos/select.h>
#include <nucleos/time.h>

unsigned sleep(unsigned sleep_seconds)
{
	struct timespec rqtp, rmtp;

	/* nanosleep implements this call; ignore failure, it cannot be reported */
	rqtp.tv_sec = sleep_seconds;
	rqtp.tv_nsec = 0;
	nanosleep(&rqtp, &rmtp);

	/* round remainder up to seconds */
	return rmtp.tv_sec + ((rmtp.tv_nsec > 0) ? 1 : 0);
}
