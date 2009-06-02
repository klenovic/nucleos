/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

/* Priority range for the get/setpriority() interface.
 * It isn't a mapping on the internal minix scheduling
 * priority.
 */
#define PRIO_MIN	-20
#define PRIO_MAX	 20

/* Magic, invalid priority to stop the process. */
#define PRIO_STOP	 76

#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2

int getpriority(int, int);
int setpriority(int, int, int);

#endif
