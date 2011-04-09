/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_PM_CONST_H
#define __SERVERS_PM_CONST_H

/* Constants used by the Process Manager. */
#define NR_PIDS		30000	/* process ids range from 0 to NR_PIDS-1.
				 * (magic constant: some old applications use
				 * a 'short' instead of pid_t.)
				 */

#define NO_PID		0	/* pid value indicating no process */
#define INIT_PID	1	/* INIT's process id number */

#define NO_TRACER	0	/* process is not being traced */

#define DUMPED		0200	/* bit set in status when core dumped */

#define MAX_SECS	(((1<<(sizeof(clock_t)*8-1))-1)/system_hz)
				/* max.secs for setitimer() ((2^31-1)/HZ) */
#define NR_ITIMERS	3	/* number of supported interval timers */
#endif /* __SERVERS_PM_CONST_H */
