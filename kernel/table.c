/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
 *    Aug 02, 2005   set privileges and minimal boot image (Jorrit N. Herder)
 *    Oct 17, 2004   updated above and tasktab comments  (Jorrit N. Herder)
 *    May 01, 2004   changed struct for system image  (Jorrit N. Herder)
 */
#include <nucleos/kipc.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>
#include <nucleos/com.h>

/* Variables relating to shutting down MINIX. */
char kernel_exception;           /* TRUE after system exceptions */
char shutdown_started;           /* TRUE after shutdowns / reboots */

/* Kernel information structures. This groups vital kernel information. */
struct kinfo kinfo;              /* kernel information for users */
struct machine machine;          /* machine information for users */
struct kmessages kmess;          /* diagnostic messages in kernel */
struct k_randomness krandom;     /* gather kernel random information */
struct loadinfo kloadinfo;       /* status of load average */
struct boot_param boot_param;    /* boot parameters */

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
int mon_return;                  /* true if we can return to monitor */
int do_serial_debug;
endpoint_t who_e;                /* message source endpoint */
int who_p;                       /* message source proc */
int sys_call_code;               /* kernel call number in SYSTEM */
time_t boottime;
char params_buffer[512];         /* boot monitor parameters */
int minix_panicing;
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
#define SMALL_STACK	(1024 * sizeof(char *))
#define IDL_S	SMALL_STACK	/* 3 intr, 3 temps, 4 db for Intel */
#define	HRD_S	NO_STACK	/* dummy task, uses kernel stack */
#define	TSK_S	SMALL_STACK	/* system and clock task */

/* Stack space for all the task stacks.  Declared as (char *) to align it. */
#define	TOT_STACK_SPACE	(IDL_S + HRD_S + (2 * TSK_S))
char *t_stack[TOT_STACK_SPACE / sizeof(char *)];
	
/* Define flags for the various process types. */
#define IDL_F 	(SYS_PROC | BILLABLE)	/* idle task is not preemptible as we
					 * don't want it to interfere with the
					 * timer tick interrupt handler code.
					 * Unlike other processes idle task is
					 * handled in a special way and is
					 * preempted always if timer tick occurs
					 * and there is another runnable process
					 */
#define TSK_F	(SYS_PROC)				/* kernel tasks */
#define SRV_F	(SYS_PROC | PREEMPTIBLE)		/* system services */
#define VM_F	(SYS_PROC)				/* vm */
#define USR_F	(BILLABLE | PREEMPTIBLE | PROC_FULLVM)	/* user processes */
#define SVM_F	(SRV_F | PROC_FULLVM)			/* servers with VM */

/* Define system call traps for the various process types. These call masks
 * determine what system call traps a process is allowed to make.
 */
#define TSK_T	(1 << KIPC_RECEIVE)			/* clock and system */
#define SRV_T	(~0)				/* system services */
#define USR_T   ((1 << KIPC_SENDREC))		/* user processes */

/* Send masks determine to whom processes can send messages or notifications. 
 * The values here are used for the processes in the boot image. We rely on 
 * the boot image table itself to match the order of the process numbers, so
 * that the send mask that is defined here can be interpreted properly.
 * Privilege structure 0 is shared by user processes. 
 */
#define s(n)	(1 << (s_nr_to_id(n)))
#define SRV_M	(~0)
#define SYS_M	(~0)
#define USR_M (s(PM_PROC_NR) | s(FS_PROC_NR) | s(RS_PROC_NR) | s(VM_PROC_NR))
#define DRV_M (USR_M | s(SYSTEM) | s(DS_PROC_NR) | s(LOG_PROC_NR) | s(TTY_PROC_NR))

/* Define kernel calls that processes are allowed to make. This is not looking
 * very nice, but we need to define the access rights on a per call basis. 
 * Note that the reincarnation server has all bits on, because it should
 * be allowed to distribute rights to services that it starts. 
 * 
 * Calls are unordered lists, converted by the kernel to bitmasks
 * once at runtime.
 */
#define FS_C SYS_KILL, SYS_VIRCOPY, SYS_SAFECOPYFROM, SYS_SAFECOPYTO, \
    SYS_VIRVCOPY, SYS_UMAP, SYS_GETINFO, SYS_EXIT, SYS_TIMES, SYS_SETALARM, \
    SYS_PRIVCTL, SYS_TRACE , SYS_SETGRANT, SYS_PROFBUF, SYS_SYSCTL
#define DRV_C	FS_C, SYS_SEGCTL, SYS_IRQCTL, SYS_INT86, SYS_DEVIO, \
	SYS_SDEVIO, SYS_VDEVIO, SYS_SETGRANT, SYS_PROFBUF, SYS_SYSCTL

static int
  fs_c[] = { FS_C },
  pm_c[] = { SYS_ALL_CALLS },
  rs_c[] = { SYS_ALL_CALLS },
  ds_c[] = { SYS_ALL_CALLS },
  vm_c[] = { SYS_ALL_CALLS },
  drv_c[] = { DRV_C },
  usr_c[] = { SYS_SYSCTL },
  tty_c[] = { DRV_C, SYS_PHYSCOPY, SYS_ABORT, SYS_IOPENABLE, SYS_READBIOS },
  mem_c[] = { DRV_C, SYS_PHYSCOPY, SYS_PHYSVCOPY, SYS_IOPENABLE };

/* The system image table lists all programs that are part of the boot image.
 * The order of the entries here MUST agree with the order of the programs
 * in the boot image and all kernel tasks must come first. Furthermore, the
 * order of the entries MUST agree with their process numbers. See above.
 *
 * Each entry provides the process number, flags, quantum size, scheduling
 * queue, allowed traps, ipc mask, and a name for the process table. The
 * initial program counter and stack size is also provided for kernel tasks.
 *
 * Note: the quantum size must be positive in all cases!
 */
#define c(calls) calls, (sizeof(calls) / sizeof((calls)[0]))
#define no_c  (int*)0, 0

struct boot_image image[] = {
	/* process nr, pc,flags, qs,  queue, stack, traps, ipcto, call,  name */
	{IDLE,               NULL, IDL_F,  0,      0, IDL_S,     0,     0,    no_c,   "idle" },
	{CLOCK,        clock_task, TSK_F,  8, TASK_Q, TSK_S, TSK_T,     0,    no_c,  "clock" },
	{SYSTEM,         sys_task, TSK_F,  8, TASK_Q, TSK_S, TSK_T,     0,    no_c, "system" },
	{HARDWARE,              0, TSK_F,  8, TASK_Q, HRD_S,     0,     0,    no_c, "kernel" },
	{PM_PROC_NR,            0, SRV_F, 32,      4,     0, SRV_T, SRV_M, c(pm_c),     "pm" },
	{FS_PROC_NR,            0, SRV_F, 32,      5,     0, SRV_T, SRV_M, c(fs_c),    "vfs" },
	{RS_PROC_NR,            0, SVM_F,  4,      4,     0, SRV_T, SYS_M, c(rs_c),     "rs" },
	{MEM_PROC_NR,           0, SVM_F,  4,      3,     0, SRV_T, SYS_M,c(mem_c), "memory" },
	{LOG_PROC_NR,           0, SRV_F,  4,      2,     0, SRV_T, SYS_M,c(drv_c),    "log" },
	{TTY_PROC_NR,   0,SVM_F,  4,      1, 0,     SRV_T, SYS_M,c(tty_c),             "tty" },
	{DS_PROC_NR,    0,SVM_F,  4,      4, 0,     SRV_T, SYS_M, c(ds_c),              "ds" },
	{MFS_PROC_NR,           0, SVM_F, 32,      5,     0, SRV_T, SRV_M, c(fs_c),    "mfs" },
	{VM_PROC_NR,            0, VM_F, 32,      2,     0, SRV_T, SRV_M, c(vm_c),     "vm" },
	{INIT_PROC_NR,          0, USR_F,  8, USER_Q,     0, USR_T, USR_M, c(usr_c),   "init" },
};

/* Verify the size of the system image table at compile time. Also verify that 
 * the first chunk of the ipc mask has enough bits to accommodate the processes
 * in the image.  
 * If a problem is detected, the size of the 'dummy' array will be negative, 
 * causing a compile time error. Note that no space is actually allocated 
 * because 'dummy' is declared extern.
 */
extern int dummy[(NR_BOOT_PROCS==sizeof(image)/
	sizeof(struct boot_image))?1:-1];
extern int dummy[(BITCHUNK_BITS > NR_BOOT_PROCS - 1) ? 1 : -1];

#ifdef CONFIG_IDLE_TSC
int idle_active;
u64_t idle_stop;
u64_t idle_tsc;
#endif
