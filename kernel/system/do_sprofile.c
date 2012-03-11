/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:    SYS_SPROFILE
 *
 * The parameters for this kernel call are:
 *    m_data1:    PROF_ACTION       (start/stop profiling)
 *    m_data2:    PROF_MEM_SIZE     (available memory for data)
 *    m_data3:    PROF_FREQ         (requested sample frequency)
 *    m_data4:    PROF_ENDPT        (endpoint of caller)
 *    m_data5:    PROF_CTL_PTR      (location of info struct)
 *    m_data6:    PROF_MEM_PTR      (location of memory for data)
 *
 * Changes:
 *   14 Aug, 2006   Created (Rogier Meurs)
 */

#include <kernel/system.h>

#ifdef CONFIG_DEBUG_KERNEL_STATS_PROFILE

/* user address to write info struct */
static vir_bytes sprof_info_addr_vir;

/*===========================================================================*
 *				do_sprofile				     *
 *===========================================================================*/
int do_sprofile(m_ptr)
register kipc_msg_t *m_ptr;    /* pointer to request message */
{
  int proc_nr, i;
  vir_bytes vir_dst;
  phys_bytes length;

  switch(m_ptr->PROF_ACTION) {

  case PROF_START:
	/* Starting profiling.
	 *
	 * Check if profiling is not already running.  Calculate physical
	 * addresses of user pointers.  Reset counters.  Start CMOS timer.
	 * Turn on profiling.
	 */
	if (sprofiling) {
		printk("SYSTEM: start s-profiling: already started\n");
		return -EBUSY;
	}

	/* Test endpoint number. */
	if(!isokendpt(m_ptr->PROF_ENDPT, &proc_nr))
		return -EINVAL;

	/* Set parameters for statistical profiler. */
	sprof_ep = m_ptr->PROF_ENDPT;
	sprof_info_addr_vir = (vir_bytes) m_ptr->PROF_CTL_PTR;
	sprof_data_addr_vir = (vir_bytes) m_ptr->PROF_MEM_PTR;

	sprof_info.mem_used = 0;
	sprof_info.total_samples = 0;
	sprof_info.idle_samples = 0;
	sprof_info.system_samples = 0;
	sprof_info.user_samples = 0;

	sprof_mem_size = m_ptr->PROF_MEM_SIZE;

	init_profile_clock(m_ptr->PROF_FREQ);
	
	sprofiling = 1;

  	return 0;

  case PROF_STOP:
	/* Stopping profiling.
	 *
	 * Check if profiling is indeed running.  Turn off profiling.
	 * Stop CMOS timer.  Copy info struct to user process.
	 */
	if (!sprofiling) {
		printk("SYSTEM: stop s-profiling: not started\n");
		return -EBUSY;
	}

	sprofiling = 0;

	stop_profile_clock();

	data_copy(SYSTEM, (vir_bytes) &sprof_info,
		sprof_ep, sprof_info_addr_vir, sizeof(sprof_info));

  	return 0;

  default:
	return -EINVAL;
  }
}

#endif /* CONFIG_DEBUG_KERNEL_STATS_PROFILE */

