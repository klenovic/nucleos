/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* Various variables could not be declared extern, but are public
 * or static. The reason for this is that extern variables cannot have a
 * default initialization. If such variables are shared, they must also be
 * declared in one of the *.h files without the initialization.  Examples
 * include 'boot_image' (this file) and 'idt' and 'gdt' (protect.c).
 *
 * Changes:
 *    Nov 22, 2009   rewrite of privilege management (Cristiano Giuffrida)
 *    Aug 02, 2005   set privileges and minimal boot image (Jorrit N. Herder)
 *    Oct 17, 2004   updated above and tasktab comments  (Jorrit N. Herder)
 *    May 01, 2004   changed struct for system image  (Jorrit N. Herder)
 */
#include <nucleos/kipc.h>
#include <nucleos/com.h>
#include <nucleos/type.h>
#include <kernel/priv.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>

/* Variables relating to shutting down MINIX. */
char kernel_exception;           /* TRUE after system exceptions */
char shutdown_started;           /* TRUE after shutdowns / reboots */

/* Kernel information structures. This groups vital kernel information. */
struct kinfo kinfo;              /* kernel information for users */
struct machine machine;          /* machine information for users */
struct kmessages kmess;          /* diagnostic messages in kernel */
struct k_randomness krandom;     /* gather kernel random information */
struct loadinfo kloadinfo;       /* status of load average */
struct boot_params boot_params;    /* boot parameters */

/* Process scheduling information and the kernel reentry count. */
struct proc *proc_ptr;   /* pointer to currently running process */
struct proc *next_ptr;   /* next process to run after restart() */
struct proc *bill_ptr;   /* process to bill for clock ticks */
struct proc *vmrestart;  /* first process on vmrestart queue */
struct proc *vmrequest;  /* first process on vmrequest queue */
struct proc *pagefaults; /* first process on pagefault queue */
struct proc *softnotify; /* first process on softnotify queue */
unsigned lost_ticks;     /* clock ticks counted outside clock task */

/* Interrupt related variables. */
irq_hook_t irq_hooks[NR_IRQ_HOOKS];      /* hooks for general use */
int irq_actids[NR_IRQ_VECTORS];          /* IRQ ID bits active */
int irq_use;                             /* map of all in-use irq's */
u32_t system_hz;                         /* HZ value */

/* Miscellaneous. */
reg_t mon_ss, mon_sp;            /* boot monitor stack */
int do_serial_debug;
endpoint_t who_e;                /* message source endpoint */
int who_p;                       /* message source proc */
int sys_call_code;               /* kernel call number in SYSTEM */
time_t boottime;
char params_buffer[512];         /* boot monitor parameters */
int kernel_in_panic = 0;
int locklevel;
u32_t magictest;

unsigned long cr3switch;
unsigned long cr3reload;

/* VM */
phys_bytes vm_base;
phys_bytes vm_size;
phys_bytes vm_mem_high;
int vm_running;
int must_notify_vm;

/* Verbose flags (debugging). */
int verbose_vm;

/* Timing */
util_timingdata_t timingdata[TIMING_CATEGORIES];

void (*level0_func)(void);

/* Define stack sizes for the kernel tasks included in the system image. */
#define NO_STACK	0
#define SMALL_STACK	(1024 * sizeof(char*))
#define IDL_S		SMALL_STACK	/* 3 intr, 3 temps, 4 db for Intel */
#define	HRD_S		NO_STACK	/* dummy task, uses kernel stack */
#define	TSK_S		SMALL_STACK	/* system and clock task */

/* Stack space for all the task stacks.  Declared as (char *) to align it. */
#define	TOT_STACK_SPACE	(IDL_S + HRD_S + (2 * TSK_S))
char *t_stack[TOT_STACK_SPACE / sizeof(char*)];

/* Define boot process flags. */
#define BVM_F	(PROC_FULLVM)                    /* boot processes with VM */

/* The system image table lists all programs that are part of the boot image.
 * The order of the entries here MUST agree with the order of the programs
 * in the boot image and all kernel tasks must come first.
 *
 * Each entry provides the process number, flags, quantum size, scheduling
 * queue, and a name for the process table. The initial program counter and
 * stack size is also provided for kernel tasks.
 *
 * Note: the quantum size must be positive in all cases!
 */

struct boot_image image[] = {
	/* process nr, pc, flags, qs,  queue, stack,   name */
	{IDLE,          0,     0,  0,      0, IDL_S, "idle"    },
	{CLOCK,clock_task,     0,  8, TASK_Q, TSK_S, "clock"   },
	{SYSTEM, sys_task,     0,  8, TASK_Q, TSK_S, "system"  },
	{HARDWARE,      0,     0,  8, TASK_Q, HRD_S, "kernel"  },
	{PM_PROC_NR,    0,     0, 32,      4, 0,     "pm"      },
	{VFS_PROC_NR,   0,     0, 32,      5, 0,     "vfs"     },
	{RS_PROC_NR,    0,     0,  4,      4, 0,     "rs"      },
	{MEM_PROC_NR,   0, BVM_F,  4,      3, 0,     "memory"  },
	{LOG_PROC_NR,   0, BVM_F,  4,      2, 0,     "log"     },
	{TTY_PROC_NR,   0, BVM_F,  4,      1, 0,     "tty"     },
	{DS_PROC_NR,    0, BVM_F,  4,      4, 0,     "ds"      },
	{MFS_PROC_NR,   0, BVM_F, 32,      5, 0,     "minixfs" },
	{VM_PROC_NR,    0,     0, 32,      2, 0,     "vm"      },
	{PFS_PROC_NR,   0, BVM_F, 32,      5, 0,     "pipefs"  },
	{EXT2_PROC_NR,  0, BVM_F, 32,      5, 0,     "ext2"    },
	{INIT_PROC_NR,  0, BVM_F,  8, USER_Q, 0,     "init"    },
};

/* Verify the size of the system image table at compile time. Also verify that 
 * the first chunk of the ipc mask has enough bits to accommodate the processes
 * in the image.  
 * If a problem is detected, the size of the 'dummy' array will be negative, 
 * causing a compile time error. Note that no space is actually allocated 
 * because 'dummy' is declared extern.
 */
extern int dummy[(NR_BOOT_PROCS==sizeof(image)/sizeof(struct boot_image))?1:-1];
extern int dummy[(BITCHUNK_BITS > NR_BOOT_PROCS - 1) ? 1 : -1];

#ifdef CONFIG_IDLE_TSC
int idle_active;
u64_t idle_stop;
u64_t idle_tsc;
#endif

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
