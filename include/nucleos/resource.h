/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_RESOURCE_H
#define __NUCLEOS_RESOURCE_H

/* Priority range for the get/setpriority() interface.
 * It isn't a mapping on the internal minix scheduling
 * priority.
 */
#define PRIO_MIN	-20
#define PRIO_MAX	 20

#define PRIO_PROCESS	0
#define PRIO_PGRP	1
#define PRIO_USER	2

#if defined(__KERNEL__) || defined(__UKERNEL__)
int getpriority(int, int);
int setpriority(int, int, int);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_RESOURCE_H */
