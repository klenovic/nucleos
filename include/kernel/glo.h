/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __KERNEL_GLO_H
#define __KERNEL_GLO_H

#ifdef __KERNEL__

#include <nucleos/sysutil.h>
#include <kernel/const.h>
#include <kernel/debug.h>
#include <asm/bootparam.h>
#include <asm/kernel/types.h>
#include <asm/kernel/glo.h>

/* Variables relating to shutting down MINIX. */
extern char kernel_exception;		/* TRUE after system exceptions */
extern char shutdown_started;		/* TRUE after shutdowns / reboots */

/* Kernel information structures. This groups vital kernel information. */
extern struct kinfo kinfo;		/* kernel information for users */
extern struct machine machine;		/* machine information for users */
extern struct kmessages kmess;  	/* diagnostic messages in kernel */
extern struct k_randomness krandom;	/* gather kernel random information */
extern struct loadinfo kloadinfo;	/* status of load average */
extern struct boot_param boot_param;	/* boot parameters */

/* Process scheduling information and the kernel reentry count. */
extern struct proc *proc_ptr;	/* pointer to currently running process */
extern struct proc *bill_ptr;	/* process to bill for clock ticks */
extern struct proc *vmrestart;  /* first process on vmrestart queue */
extern struct proc *vmrequest;  /* first process on vmrequest queue */
extern struct proc *pagefaults; /* first process on pagefault queue */
extern unsigned lost_ticks;	/* clock ticks counted outside clock task */

/* Interrupt related variables. */
extern irq_hook_t irq_hooks[NR_IRQ_HOOKS];	/* hooks for general use */
extern int irq_actids[NR_IRQ_VECTORS];		/* IRQ ID bits active */
extern int irq_use;				/* map of all in-use irq's */
extern u32_t system_hz;				/* HZ value */

/* Miscellaneous. */
extern reg_t mon_ss, mon_sp;		/* boot monitor stack */
extern int mon_return;			/* true if we can return to monitor */
extern int do_serial_debug;
extern endpoint_t who_e;		/* message source endpoint */
extern int who_p;			/* message source proc */
extern int sys_call_code;		/* kernel call number in SYSTEM */
extern time_t boottime;
extern char params_buffer[512];		/* boot monitor parameters */
extern int minix_panicing;
extern int locklevel;
#define MAGICTEST 0xC0FFEE23
extern u32_t magictest;			/* global magic number */

#ifdef CONFIG_DEBUG_KERNEL_TRACE
extern int verboseflags;
#endif

#ifdef CONFIG_X86_LOCAL_APIC
extern int config_no_apic; /* optionaly turn off apic */
#endif

#ifdef CONFIG_IDLE_TSC
extern u64_t idle_tsc;
extern u64_t idle_stop;
extern int idle_active;
#endif

/* VM */
extern int vm_running;
extern int catch_pagefaults;
extern struct proc *ptproc;

/* Timing */
extern util_timingdata_t timingdata[TIMING_CATEGORIES];

/* Variables that are initialized elsewhere are just extern here. */
extern struct boot_image image[]; 	/* system image processes */
extern char *t_stack[];			/* task stack space */
extern struct segdesc_s gdt[];		/* global descriptor table */

#endif /* __KERNEL__ */
#endif /* __KERNEL_GLO_H */
