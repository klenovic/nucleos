/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef PROFILE_H
#define PROFILE_H

#include <nucleos/profile.h>

#ifdef __KERNEL__

#ifdef CONFIG_DEBUG_KERNEL_STATS_PROFILE	/* statistical profiling */

EXTERN int sprofiling;			/* whether profiling is running */
EXTERN int sprof_mem_size;		/* available user memory for data */
EXTERN struct sprof_info_s sprof_info;	/* profiling info for user program */
EXTERN vir_bytes sprof_data_addr_vir;	/* user address to write data */
EXTERN endpoint_t sprof_ep;		/* user process */

#endif /* CONFIG_DEBUG_KERNEL_STATS_PROFILE */


EXTERN int cprof_mem_size;		/* available user memory for data */
EXTERN struct cprof_info_s cprof_info;	/* profiling info for user program */
EXTERN int cprof_procs_no;		/* number of profiled processes */
EXTERN struct cprof_proc_info_s {	/* info about profiled process */
	endpoint_t endpt;		/* endpoint */
	char *name;			/* name */
	vir_bytes ctl_v;		/* location of control struct */
	vir_bytes buf_v;		/* location of buffer */
	int slots_used;			/* table slots used */
} cprof_proc_info_inst;
EXTERN struct cprof_proc_info_s cprof_proc_info[NR_SYS_PROCS];	

#endif /* __KERNEL__ */
#endif /* PROFILE_H */

