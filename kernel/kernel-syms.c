/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/type.h>
#include <kernel/proc.h>
#include <kernel/priv.h>

/* The process table and pointers to process table slots. The pointers allow
 * faster access because now a process entry can be found by indexing the
 * pproc_addr array, while accessing an element i requires a multiplication
 * with sizeof(struct proc) to determine the address.
 */
struct proc proc[NR_TASKS + NR_PROCS];	/* process table */
struct proc *rdy_head[NR_SCHED_QUEUES];	/* ptrs to ready list headers */
struct proc *rdy_tail[NR_SCHED_QUEUES];	/* ptrs to ready list tails */

/* The system structures table and pointers to individual table slots. The
 * pointers allow faster access because now a process entry can be found by
 * indexing the psys_addr array, while accessing an element i requires a
 * multiplication with sizeof(struct sys) to determine the address.
 */
struct priv priv[NR_SYS_PROCS];		/* system properties table */
struct priv *ppriv_addr[NR_SYS_PROCS];	/* direct slot pointers */
