/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the table used to map system call numbers onto the
 * routines that perform them.
 */
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include "fs.h"
#include "file.h"
#include "fproc.h"
#include "lock.h"
#include "vnode.h"
#include "vmnt.h"

/* File System global variables */
struct fproc *fp;        /* pointer to caller's fproc struct */
int super_user;          /* 1 if caller is super_user, else 0 */
int susp_count;          /* number of procs suspended on pipe */
int nr_locks;            /* number of locks currently in place */
int reviving;            /* number of pipe processes to be revived */

dev_t root_dev;          /* device number of the root device */
int ROOT_FS_E;           /* kernel endpoint of the root FS_PROC_NR proc */
int last_login_fs_e;     /* endpoint of the FS_PROC_NR proc that logged in
                                   before the corresponding mount request */
u32_t system_hz;         /* system clock frequency. */

/* The parameters of the call are kept here. */
message m_in;            /* the input message itself */
message m_out;           /* the output message used for reply */
int who_p, who_e;        /* caller's proc number, endpoint */
int call_nr;             /* system call number */
message mount_m_in;      /* the input message itself */

char user_fullpath[PATH_MAX+1];    /* storage for user path name */
short cum_path_processed;        /* number of characters processed */

/* The following variables are used for returning results to the caller. */
int err_code;            /* temporary storage for error number */

#define SCALL_HANDLER(syscall, handler) \
	[ __NNR_ ## syscall ] = handler

int (*call_vec[])(void) = {
	no_sys,		/*  0 = unused */
	no_sys,		/*  1 = (exit) */
	no_sys,		/*  2 = (fork) */
	do_read,	/*  3 = read */
	do_write,	/*  4 = write */
	do_open,	/*  5 = open */
	do_close,	/*  6 = close */
	no_sys,		/*  7 = wait */
	do_creat,	/*  8 = creat */
	do_link,	/*  9 = link */
	do_unlink,	/* 10 = unlink */
	no_sys,		/* 11 = waitpid */
	do_chdir,	/* 12 = chdir */
	no_sys,		/* 13 = time */
	do_mknod,	/* 14 = mknod */
	do_chmod,	/* 15 = chmod */
	do_chown,	/* 16 = chown */
	no_sys,		/* 17 = break */
	do_stat,	/* 18 = stat */
	do_lseek,	/* 19 = lseek */
	no_sys,		/* 20 = getpid */
	do_mount,	/* 21 = mount */
	do_umount,	/* 22 = umount */
	no_sys,		/* 23 = (setuid) */
	no_sys,		/* 24 = getuid */
	no_sys,		/* 25 = (stime) */
	no_sys,		/* 26 = ptrace */
	no_sys,		/* 27 = alarm */
	do_fstat,	/* 28 = fstat */
	no_sys,		/* 29 = pause */
	do_utime,	/* 30 = utime */
	no_sys,		/* 31 = (stty) */
	no_sys,		/* 32 = (gtty) */
	do_access,	/* 33 = access */
	no_sys,		/* 34 = (nice) */
	no_sys,		/* 35 = (ftime) */
	do_sync,	/* 36 = sync */
	no_sys,		/* 37 = kill */
	do_rename,	/* 38 = rename */
	do_mkdir,	/* 39 = mkdir */
	do_unlink,	/* 40 = rmdir */
	do_dup,		/* 41 = dup */
	do_pipe,	/* 42 = pipe */
	no_sys,		/* 43 = times */
	no_sys,		/* 44 = (prof) */
	do_slink,	/* 45 = symlink */
	no_sys,		/* 46 = (setgid) */
	no_sys,		/* 47 = getgid */
	no_sys,		/* 48 = (signal)*/
	do_rdlink,	/* 49 = readlink*/
	do_lstat,	/* 50 = lstat */
	no_sys,		/* 51 = (acct) */
	no_sys,		/* 52 = (phys) */
	no_sys,		/* 53 = (lock) */
	do_ioctl,	/* 54 = ioctl */
	do_fcntl,	/* 55 = fcntl */
	no_sys,		/* 56 = (mpx) */
	do_fslogin,	/* 57 = FS_PROC_NR proc login */
	no_sys,		/* 58 = unused */
	no_sys,		/* 59 = (execve) */
	do_umask,	/* 60 = umask */
	do_chroot,	/* 61 = chroot */
	no_sys,		/* 62 = (setsid) */
	no_sys,		/* 63 = (getpgrp) */
	no_sys,		/* 64 = (itimer) */
	no_sys,		/* 65 = unused */
	no_sys, 	/* 66 = unused  */
	no_sys,		/* 67 = unused */
	no_sys,		/* 68 = unused */
	no_sys,		/* 69 = unused */
	no_sys,		/* 70 = unused */
	no_sys,		/* 71 = (sigaction) */
	no_sys,		/* 72 = (sigsuspend) */
	no_sys,		/* 73 = (sigpending) */
	no_sys,		/* 74 = (sigprocmask) */
	no_sys,		/* 75 = (sigreturn) */
	no_sys,		/* 76 = (reboot) */
	do_svrctl,	/* 77 = svrctl */
	no_sys,		/* 78 = (sysuname) */
	do_getsysinfo,	/* 79 = getsysinfo */
	do_getdents,	/* 80 = getdents */
	do_llseek,	/* 81 = llseek */
	do_fstatfs,	/* 82 = fstatfs */
	no_sys,		/* 83 = unused */
	no_sys,		/* 84 = unused */
	do_select,	/* 85 = select */
	do_fchdir,	/* 86 = fchdir */
	do_fsync,	/* 87 = fsync */
	no_sys,		/* 88 = (getpriority) */
	no_sys,		/* 89 = (setpriority) */
	no_sys,		/* 90 = (gettimeofday) */
	no_sys,		/* 91 = (seteuid) */
	no_sys,		/* 92 = (setegid) */
	do_truncate,	/* 93 = truncate */
	do_ftruncate,	/* 94 = truncate */
	do_chmod,	/* 95 = fchmod */
	do_chown,	/* 96 = fchown */
	no_sys,		/* 97 = getsysinfo_up */
	no_sys,		/* 98 = (sprofile) */
	no_sys,		/* 99 = (cprofile) */
	no_sys,		/* 100 = unused */
	no_sys,		/* 101 = unused */
	no_sys,		/* 102 = unused */
	no_sys,		/* 103 = unused */
	no_sys,		/* 104 = unused */
	no_sys,		/* 105 = unused */
	no_sys,		/* 106 = unused */
	no_sys,		/* 107 = unused */
	no_sys,		/* 108 = (adddma) */
	no_sys,		/* 109 = unused */
	no_sys,		/* 110 = unused */

	/* Nucleos syscalls */
	SCALL_HANDLER(access,		do_access),
	SCALL_HANDLER(chdir,		do_chdir),
	SCALL_HANDLER(chmod,		do_chmod),
	SCALL_HANDLER(chown,		do_chown),
	SCALL_HANDLER(chroot,		do_chroot),
	SCALL_HANDLER(close,		do_close),
	SCALL_HANDLER(creat,		do_creat),
	SCALL_HANDLER(dup,		do_dup),
	SCALL_HANDLER(dup2,		do_dup2),
	SCALL_HANDLER(fchdir,		do_fchdir),
	SCALL_HANDLER(fchmod,		do_fchmod),
	SCALL_HANDLER(fchown,		do_fchown),
	SCALL_HANDLER(fcntl,		do_fcntl),
	SCALL_HANDLER(fstat,		do_fstat),
	SCALL_HANDLER(fstatfs,		do_fstatfs),
	SCALL_HANDLER(fsync,		do_fsync),
	SCALL_HANDLER(ftruncate,	no_sys),
	SCALL_HANDLER(getdents,		no_sys),
	SCALL_HANDLER(ioctl,		no_sys),
	SCALL_HANDLER(link,		no_sys),
	SCALL_HANDLER(llseek,		no_sys),
	SCALL_HANDLER(lseek,		no_sys),
	SCALL_HANDLER(lstat,		no_sys),
	SCALL_HANDLER(mkdir,		no_sys),
	SCALL_HANDLER(mknod,		no_sys),
	SCALL_HANDLER(mount,		no_sys),
	SCALL_HANDLER(open,		no_sys),
	SCALL_HANDLER(pipe,		no_sys),
	SCALL_HANDLER(read,		no_sys),
	SCALL_HANDLER(readlink,		no_sys),
	SCALL_HANDLER(rename,		no_sys),
	SCALL_HANDLER(rmdir,		no_sys),
	SCALL_HANDLER(select,		no_sys),
	SCALL_HANDLER(stat,		no_sys),
	SCALL_HANDLER(symlink,		no_sys),
	SCALL_HANDLER(sync,		no_sys),
	SCALL_HANDLER(truncate,		no_sys),
	SCALL_HANDLER(umask,		no_sys),
	SCALL_HANDLER(umount,		no_sys),
	SCALL_HANDLER(unlink,		no_sys),
	SCALL_HANDLER(utime,		no_sys),
	SCALL_HANDLER(write,		no_sys),
};

/* This should not fail with "array size is negative": */
extern int dummy[sizeof(call_vec) <= NR_syscalls * sizeof(call_vec[0]) ? 1 : -1];
