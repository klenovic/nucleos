/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Dispatch the incomming syscall to appropriate server and setup arguments */
#include <nucleos/kipc.h>
#include <nucleos/ptrace.h>
#include <nucleos/unistd.h>
#include <nucleos/types.h>
#include <kernel/proc.h>
#include <kernel/glo.h>

/*
 * @nucleos: Ideally the C library should know _nothing_ about character of kernel i.e. whether
 *           it's microkernel or monolitic nor who to ask for the service. The C library should
 *           know only the system call number and the kernel shall dispatch the request to the
 *           appropriate server.
 *
 *           Maybe it make sense to create separate server<->kernel<->driver (internal)
 *           communication and separate libc<->kernel (external) communication.
 *           It allows:
 *               - to handle "pure" user (C library) request independently from the system. The
 *                 kIPC implemenentation doesn't have to change.
 *               - the things regarding messages (message format, passing, dispatching, ...) to
 *                 be handled differently for C library.
 *               - the current implementation to be less affected.
 */

struct endpt_args {
	endpoint_t endpt;
	void (*fill_msg)(message *msg, struct pt_regs *r);
};

struct endpt_args scall_endpt_args[];

__attribute__((regparm(0))) endpoint_t map_scall_endpt(message *msg, struct pt_regs *r)
{
	message kmsg;
	/* this code right below totaly kill the kernel!!! */
	/* kprintf("ax=%d bx=%d cx=%d dx=%d si=%si di=%d\n", r->ax, r->bx, r->cx, r->dx,
		    r->si, r->di); */

	kmsg.m_type = r->ax;

	/* fill the message according to passed arguments */
	if (scall_endpt_args[r->ax].fill_msg)
		scall_endpt_args[r->ax].fill_msg(&kmsg, r);

	/* @nucleos: This mapping _must_ change in the future. It is here because of current
 		     current kIPC. It requires to have message in caller's (user's) space. */

	/* Map the message from user space. */
	phys_bytes linaddr = umap_local(proc_ptr, D, (vir_bytes)msg, sizeof(message));

	/* copy the filled message into caller (user) space */
	phys_copy(vir2phys(&kmsg), linaddr, sizeof(message));

	return scall_endpt_args[r->ax].endpt;
}

static void msg_read(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */
	msg->m1_i2 = r->dx;		/* count */
}

static void msg_exit(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;
}

static void msg_ptrace(message *msg, struct pt_regs *r)
{
	msg->m2_i1 = r->cx;	/* pid */
	msg->m2_i2 = r->bx;	/* req */
	msg->PMTRACE_ADDR = r->dx;	/* addr */
	msg->m2_l2 = r->si;		/* data */
}

struct endpt_args scall_endpt_args[] = {
	[__NR_getdma]		= { PM_PROC_NR, 0, },
	/* [VM_RS_SET_PRIV]	= { VM_PROC_NR, 0, }, (RS: internal) */
	/* [VM_ADDDMA]		= { VM_PROC_NR, 0, }, (PM: internal) */
	/* [VM_DELDMA]		= { VM_PROC_NR, 0, }, (PM: internal) */
	/* [VM_GETDMA]		= { VM_PROC_NR, 0, }, (PM: internal) */
	[__NR_getprocnr]	= { PM_PROC_NR, 0, },
	[__NR_deldma]		= { PM_PROC_NR, 0, },
	/* [__NR_freemem]	= { PM_PROC_NR, 0, }, (not used) */
	[__NR_llseek]		= { FS_PROC_NR, 0, },
	[__NR_getpuid]		= { PM_PROC_NR, 0, },
	[__NR_reboot]		= { PM_PROC_NR, 0, },
	[__NR_getpid]		= { PM_PROC_NR, 0, },
	[__NR_procstat]		= { PM_PROC_NR, 0, },
	[__NR_mapdriver]	= { FS_PROC_NR, 0, },
	[__NR_brk]		= { PM_PROC_NR, 0, },
	[__NR_getgid]		= { PM_PROC_NR, 0, },
	[__NR_getuid]		= { PM_PROC_NR, 0, },
	[__NR_sprof]		= { PM_PROC_NR, 0, },
	/* issue: two different enpoints
	[__NR_svrctl]		= { PM_PROC_NR, 0, },
	[__NR_svrctl]		= { FS_PROC_NR, 0, },
	 */
	[__NR_sysuname]		= { PM_PROC_NR, 0, },
	[__NR_getdents]		= { FS_PROC_NR, 0, },
	[__NR_cprof]		= { PM_PROC_NR, 0, },
	[__NR_mount]		= { FS_PROC_NR, 0, },
	[__NR_umount]		= { FS_PROC_NR, 0, },
	/* [__NR_getsysinfo]	= ? (PM/VM/VFS issue) */
	/* [__NR_getsysinfo_up]	= ? (PM/VM/VFS issue) */
	[__NR_adddma]		= { PM_PROC_NR, 0, },
	/* VM_QUERY_EXIT]	= { VM_PROC_NR, 0, }, */
	[__NR_devctl]		= { FS_PROC_NR, 0, },
	/* MSTATS]		= { PM_PROC_NR, 0, }, (never used) */
	/* IPC_SHMCTL]		= ? (add later) */
	/* IPC_SHMGET]		= ? (add later) */
	/* IPC_SHMAT]		= ? (add later) */
	/* IPC_SHMDT]		= ? (add later) */
	/* IPC_SEMGET]		= ? (add later) */
	/* IPC_SEMCTL]		= ? (add later) */
	/* IPC_SEMOP]		= ? (add later) */
	[__NR_fork]		= { PM_PROC_NR, 0, },
	[__NR_close]		= { FS_PROC_NR, 0, },
	[__NR_setuid]		= { PM_PROC_NR, 0, },
	[__NR_seteuid]		= { PM_PROC_NR, 0, },
	[__NR_pause]		= { PM_PROC_NR, 0, },
	[__NR_itimer]		= { PM_PROC_NR, 0, },
	[__NR_write]		= { FS_PROC_NR, 0, },
	[__NR_unlink]		= { FS_PROC_NR, 0, },
	[__NR_chown]		= { FS_PROC_NR, 0, },
	[__NR_lstat]		= { FS_PROC_NR, 0, },
	[__NR_mknod]		= { FS_PROC_NR, 0, },
	[__NR_getpgrp]		= { PM_PROC_NR, 0, },
	[__NR_pipe]		= { FS_PROC_NR, 0, },
	[__NR_umask]		= { FS_PROC_NR, 0, },
	[__NR_fchmod]		= { FS_PROC_NR, 0, },
	[__NR_mkdir]		= { FS_PROC_NR, 0, },
	[__NR_alarm]		= { PM_PROC_NR, 0, },
	/* [__NR_ptrace]		= { PM_PROC_NR, msg_ptrace, }, (doesn't work correctly ) */
	[__NR_link]		= { FS_PROC_NR, 0, },
	/* [VM_MMAP]		= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_MUNMAP]		= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_MUNMAP_TEXT]	= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_REMAP]		= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_SHM_UNMAP]	= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_GETPHYS]		= { VM_PROC_NR, 0, }, (add later) */
	/* [VM_GETREF]		= { VM_PROC_NR, 0, }, (add later) */
	[__NR_fstat]		= { FS_PROC_NR, 0, },
	[__NR_sigprocmask]	= { PM_PROC_NR, 0, },
	[__NR_getpriority]	= { PM_PROC_NR, 0, },
	[__NR_setpriority]	= { PM_PROC_NR, 0, },
	[__NR_read]		= { FS_PROC_NR, msg_read, },
	[__NR_setgid]		= { PM_PROC_NR, 0, },
	[__NR_setegid]		= { PM_PROC_NR, 0, },
	[__NR_creat]		= { FS_PROC_NR, 0, },
	[__NR_fstatfs]		= { FS_PROC_NR, 0, },
	[__NR_sigsuspend]	= { PM_PROC_NR, 0, },
	[__NR_time]		= { PM_PROC_NR, 0, },
	[__NR_times]		= { PM_PROC_NR, 0, },
	[__NR_stime]		= { PM_PROC_NR, 0, },
	[__NR_fsync]		= { FS_PROC_NR, 0, },
	[__NR_rmdir]		= { FS_PROC_NR, 0, },
	[__NR_fcntl]		= { FS_PROC_NR, 0, },
	[__NR_gettimeofday]	= { PM_PROC_NR, 0, },
	[__NR_chmod]		= { FS_PROC_NR, 0, },
	[__NR_utime]		= { FS_PROC_NR, 0, },
	[__NR_lseek]		= { FS_PROC_NR, 0, },
	[__NR_exit]		= { PM_PROC_NR, msg_exit, },
	[__NR_sigpending]	= { PM_PROC_NR, 0, },
	[__NR_sigreturn]	= { PM_PROC_NR, 0, },
	[__NR_chdir]		= { FS_PROC_NR, 0, },
	[__NR_fchdir]		= { FS_PROC_NR, 0, },
	[__NR_wait]		= { PM_PROC_NR, 0, },
	[__NR_sigaction]	= { PM_PROC_NR, 0, },
	[__NR_sync]		= { FS_PROC_NR, 0, },
	[__NR_select]		= { FS_PROC_NR, 0, },
	[__NR_exec]		= { PM_PROC_NR, 0, },
	[__NR_readlink]		= { FS_PROC_NR, 0, },
	[__NR_setsid]		= { PM_PROC_NR, 0, },
	[__NR_ioctl]		= { FS_PROC_NR, 0, },
	[__NR_truncate]		= { FS_PROC_NR, 0, },
	[__NR_ftruncate]	= { FS_PROC_NR, 0, },
	[__NR_access]		= { FS_PROC_NR, 0, },
	[__NR_kill]		= { PM_PROC_NR, 0, },
	[__NR_rename]		= { FS_PROC_NR, 0, },
	[__NR_symlink]		= { FS_PROC_NR, 0, },
	[__NR_stat]		= { FS_PROC_NR, 0, },
	[__NR_open]		= { FS_PROC_NR, 0, },
	[__NR_fchown]		= { FS_PROC_NR, 0, },
	[__NR_chroot]		= { FS_PROC_NR, 0, },
	[__NR_waitpid]		= { PM_PROC_NR, 0, },
	[__NR_fork_nb]		= { PM_PROC_NR, 0, },
	[__NR_allocmem]		= { PM_PROC_NR, 0, },
};
