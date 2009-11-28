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
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <kernel/proc.h>
#include <kernel/proto.h>
#include <kernel/glo.h>
#include <kernel/vm.h>
#include <asm/servers/vm/memory.h>

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

static struct endpt_args scall_to_srv[];

#define SCALL_TO_SRV(syscall, server) \
	[ __NNR_ ## syscall ] = { server ## _PROC_NR, msg_ ## syscall }

static inline long strnlen_user(const char *s, size_t maxlen)
{
	char name[PATH_MAX];
	unsigned long len = 0;
	phys_bytes linaddr;

	/* We must not cross the top of stack during copy */
	len = ((VM_STACKTOP - (unsigned long)s) < maxlen) ? (VM_STACKTOP - (unsigned long)s) : maxlen;

	/* Map the string from user space.
	 * @nucleos: Fix this let `fetch_name' takes care.
	 */
	if (!(linaddr = umap_local(proc_ptr, D, (vir_bytes)s, len))) {
		return -EFAULT;
	}

	phys_copy(linaddr, vir2phys((vir_bytes)name), len);

	return strnlen(name, len);
}

endpoint_t map_scall_endpt(message *msg, struct pt_regs *r)
{
	message kmsg;

	memset(&kmsg, 0, sizeof(message));

	/* this code right below totaly kill the kernel!!! */
	/* kprintf("ax=%d bx=%d cx=%d dx=%d si=%si di=%d\n", r->ax, r->bx, r->cx, r->dx,
		    r->si, r->di); */

	/* @nucleos: The syscall numbers used by user (_NR_*) and kernel may be different.
	 *           E.g. user calls mmap via __NR_mmap but system knows it as VM_MMAP. In
	 *           this case the m_type can be changed to appropriate number in msg_* function.
	 *           Thing to change ???
	 */
	kmsg.m_type = r->ax;

	/* fill the message according to passed arguments */
	if (scall_to_srv[r->ax].fill_msg)
		scall_to_srv[r->ax].fill_msg(&kmsg, r);

	/* @nucleos: This mapping _must_ change in the future. It is here because of current
		     kIPC. It requires to have message in caller's (user's) space. */

	/* Map the message from user space. */
	phys_bytes linaddr = umap_local(proc_ptr, D, (vir_bytes)msg, sizeof(message));

	/* copy the filled message into caller (user) space */
	phys_copy(vir2phys(&kmsg), linaddr, sizeof(message));

	return scall_to_srv[r->ax].endpt;
}

static void msg_access(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m3_i2 = r->cx;		/* mode */
}

static void msg_alarm(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* seconds */
}

static void msg_brk(message *msg, struct pt_regs *r)
{
	msg->PMBRK_ADDR = (void*)r->bx;		/* addr */
}

static void msg_chdir(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
}

static void msg_chmod(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m3_i2 = (mode_t)r->cx;	/* mode */
}

static void msg_chown(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* name */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_i2 = (uid_t)r->cx;	/* owner */
	msg->m1_i3 = (gid_t)r->dx;	/* group */
}

static void msg_chroot(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* path */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
}

static void msg_close(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;
}

static void msg_cprof(message *msg, struct pt_regs *r)
{
	msg->PROF_ACTION	= r->bx;		/* action */
	msg->PROF_MEM_SIZE	= r->cx;		/* size */
	msg->PROF_CTL_PTR	= (void *)r->dx;	/* ctl_ptr */
	msg->PROF_MEM_PTR	= (void *)r->si;	/* mem_ptr */
}

static void msg_creat(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m3_i2 = (mode_t)r->cx;	/* mode */
}

static void msg_dup(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */
}

static void msg_dup2(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */
	msg->m1_i2 = r->cx;	/* descriptor */
}

static void msg_exec(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* filename */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (char*)r->cx;	/* frame */
	msg->m1_i2 = r->dx;		/* frame_size */
}

static void msg_exit(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;
}

static void msg_fchdir(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */
}

static void msg_fchmod(message *msg, struct pt_regs *r)
{
	msg->m3_i1 = r->bx;		/* descriptor */
	msg->m3_i2 = (mode_t)r->cx;	/* mode */
}

static void msg_fchown(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_i2 = (uid_t)r->cx;	/* owner */
	msg->m1_i3 = (gid_t)r->dx;	/* group */
}

static void msg_fcntl(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_i2 = r->cx;		/* cmd */
	msg->m1_i3 = r->dx;		/* flags */
	msg->m1_p1 = (void*)r->dx;	/* lock */
}

static void msg_fork(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_fstat(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */
}

static void msg_fstatfs(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */
}

static void msg_fsync(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */
}

static void msg_ftruncate(message *msg, struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l1 = (off_t)r->cx;	/* length */
}

static void msg_getdents(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* struct. dirent */
	msg->m1_i2 = (size_t)r->dx;		/* count */
}

static void msg_getegid(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_getgid(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_getitimer(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* which */
	msg->m1_p2 = (void*)r->cx;	/* value */
}

static void msg_getpgrp(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_getpid(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_getppid(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_getpriority(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* which */
	msg->m1_i2 = r->cx;	/* who */
}

static void msg_gettimeofday(message *msg, struct pt_regs *r){}
static void msg_getuid(message *msg, struct pt_regs *r){}

static void msg_ioctl(message *msg, struct pt_regs *r)
{
	msg->TTY_LINE = r->bx;		/* descriptor */
	msg->TTY_REQUEST = r->cx;	/* request */
	msg->ADDRESS = (char *)r->dx;	/* data */
}

static void msg_kill(message *msg, struct pt_regs *r){}

static void msg_link(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;

	msg->m1_p2 = (char*)r->cx;	/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX) + 1;
}

static void msg_llseek(message *msg, struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l2 = r->cx;		/* offset (high) */
	msg->m2_l1 = r->dx;		/* offset (low) */
	msg->m2_p1 = (void*)r->si;	/* result (address) */
	msg->m2_i2 = r->di;		/* whence */
}

static void msg_lseek(message *msg, struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l1 = (off_t)r->cx;	/* offset */
	msg->m2_i2 = r->dx;		/* whence */
}

static void msg_lstat(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (char*)r->cx;	/* buffer */
}

static void msg_mkdir(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* name */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_i2 = (mode_t)r->cx;	/* mode */
}

static void msg_mknod(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *) r->bx;		/* pathname */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_i2 = (mode_t)r->cx;		/* mode */
	msg->m1_i3 = (dev_t)r->dx;		/* device */
	msg->m1_p2 = (char *) ((int) 0);	/* obsolete size field */
}

static void msg_mount(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* special */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;

	msg->m1_p2 = (char *)r->cx;	/* name */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX) + 1;

	msg->m1_i3 = r->dx;		/* mountflags */
	msg->m1_p3 = (char *)r->si;	/* ep */
}

static void msg_open(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* pathname */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_i2 = r->cx;		/* flags */
	msg->m1_i3 = (mode_t)r->dx;	/* mode */
}

static void msg_pause(message *msg, struct pt_regs *r){}

static void msg_pipe(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* pointer to fild */
}

static void msg_ptrace(message *msg, struct pt_regs *r){}

static void msg_read(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */
	msg->m1_i2 = r->dx;		/* count */
}

static void msg_readlink(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (void *)r->cx;	/* buffer */
	msg->m1_i2 = r->dx;		/* bufsiz */
}

static void msg_reboot(message *msg, struct pt_regs *r){}

static void msg_rename(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (void *)r->cx;	/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX) + 1;
}

static void msg_rmdir(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
}

static void msg_select(message *msg, struct pt_regs *r)
{
	msg->SEL_NFDS = r->bx;			/* nfds */
	msg->SEL_READFDS = (char *) r->cx;	/* readfds */
	msg->SEL_WRITEFDS = (char *) r->dx;	/* writefds */
	msg->SEL_ERRORFDS = (char *) r->si;	/* errorfds */
	msg->SEL_TIMEOUT = (char *) r->di;	/* timeout */
}

static void msg_setegid(message *msg, struct pt_regs *r){}
static void msg_seteuid(message *msg, struct pt_regs *r){}
static void msg_setgid(message *msg, struct pt_regs *r){}
static void msg_setitimer(message *msg, struct pt_regs *r){}
static void msg_setpriority(message *msg, struct pt_regs *r){}
static void msg_setsid(message *msg, struct pt_regs *r){}
static void msg_setuid(message *msg, struct pt_regs *r){}
static void msg_sigaction(message *msg, struct pt_regs *r){}
static void msg_signal(message *msg, struct pt_regs *r){}
static void msg_sigpending(message *msg, struct pt_regs *r){}
static void msg_sigprocmask(message *msg, struct pt_regs *r){}
static void msg_sigreturn(message *msg, struct pt_regs *r){}
static void msg_sigsuspend(message *msg, struct pt_regs *r){}
static void msg_sprof(message *msg, struct pt_regs *r){}

static void msg_stat(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (void *)r->cx;	/* buffer */
}

static void msg_stime(message *msg, struct pt_regs *r){}

static void msg_symlink(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;		/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (char *)r->cx;		/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX) + 1;
}

static void msg_sync(message *msg, struct pt_regs *r)
{
	/* no args */
}

static void msg_sysuname(message *msg, struct pt_regs *r){}
static void msg_time(message *msg, struct pt_regs *r){}
static void msg_times(message *msg, struct pt_regs *r){}

static void msg_truncate(message *msg, struct pt_regs *r)
{
	msg->m2_p1 = (char *)r->bx;		/* path */
	msg->m2_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m2_l1 = (off_t)r->cx;		/* length */
}

static void msg_umask(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = (mode_t)r->bx;	/* mask */
}

static void msg_umount(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* target */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
}

static void msg_unlink(message *msg, struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
}

static void msg_utime(message *msg, struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* filename */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX) + 1;
	msg->m1_p2 = (void *)r->cx;	/* times pointer */
}

static void msg_wait(message *msg, struct pt_regs *r){}
static void msg_waitpid(message *msg, struct pt_regs *r){}

static void msg_write(message *msg, struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (char *)r->cx;	/* buffer */
	msg->m1_i2 = (size_t)r->dx;	/* count */
}

static struct endpt_args scall_to_srv[] = {
	SCALL_TO_SRV(access,		FS),
	SCALL_TO_SRV(alarm,		PM),
	SCALL_TO_SRV(brk,		PM),
	SCALL_TO_SRV(chdir,		FS),
	SCALL_TO_SRV(chmod,		FS),
	SCALL_TO_SRV(chown,		FS),
	SCALL_TO_SRV(chroot,		FS),
	SCALL_TO_SRV(close,		FS),
	SCALL_TO_SRV(cprof,		PM),
	SCALL_TO_SRV(creat,		FS),	/* 10 */

	SCALL_TO_SRV(dup,		FS),
	SCALL_TO_SRV(dup2,		FS),
	SCALL_TO_SRV(exec,		PM),
	SCALL_TO_SRV(exit,		PM),
	SCALL_TO_SRV(fchdir,		FS),
	SCALL_TO_SRV(fchmod,		FS),
	SCALL_TO_SRV(fchown,		FS),
	SCALL_TO_SRV(fcntl,		FS),
	SCALL_TO_SRV(fork,		PM),
	SCALL_TO_SRV(fstat,		FS),	/* 20 */

	SCALL_TO_SRV(fstatfs,		FS),
	SCALL_TO_SRV(fsync,		FS),
	SCALL_TO_SRV(ftruncate,		FS),
	SCALL_TO_SRV(getdents,		FS),
	SCALL_TO_SRV(getegid,		PM),
	SCALL_TO_SRV(getgid,		PM),
	SCALL_TO_SRV(getitimer,		PM),
	SCALL_TO_SRV(getpgrp,		PM),
	SCALL_TO_SRV(getpid,		PM),
	SCALL_TO_SRV(getppid,		PM),	/* 30 */

	SCALL_TO_SRV(getpriority,	PM),
	SCALL_TO_SRV(gettimeofday,	PM),
	SCALL_TO_SRV(getuid,		PM),
	SCALL_TO_SRV(ioctl,		FS),
	SCALL_TO_SRV(kill,		PM),
	SCALL_TO_SRV(link,		FS),
	SCALL_TO_SRV(llseek,		FS),
	SCALL_TO_SRV(lseek,		FS),
	SCALL_TO_SRV(lstat,		FS),
	SCALL_TO_SRV(mkdir,		FS),	/* 40 */

	SCALL_TO_SRV(mknod,		FS),
	SCALL_TO_SRV(mount,		FS),
	SCALL_TO_SRV(open,		FS),
	SCALL_TO_SRV(pause,		PM),
	SCALL_TO_SRV(pipe,		FS),
	SCALL_TO_SRV(ptrace,		PM),
	SCALL_TO_SRV(read,		FS),
	SCALL_TO_SRV(readlink,		FS),
	SCALL_TO_SRV(reboot,		PM),
	SCALL_TO_SRV(rename,		FS),	/* 50 */

	SCALL_TO_SRV(rmdir,		FS),
	SCALL_TO_SRV(select,		FS),
	SCALL_TO_SRV(setegid,		PM),
	SCALL_TO_SRV(seteuid,		PM),
	SCALL_TO_SRV(setgid,		PM),
	SCALL_TO_SRV(setitimer,		PM),
	SCALL_TO_SRV(setpriority,	PM),
	SCALL_TO_SRV(setsid,		PM),
	SCALL_TO_SRV(setuid,		PM),
	SCALL_TO_SRV(sigaction,		PM),	/* 60 */

	SCALL_TO_SRV(signal,		PM),
	SCALL_TO_SRV(sigpending,	PM),
	SCALL_TO_SRV(sigprocmask,	PM),
	SCALL_TO_SRV(sigreturn,		PM),
	SCALL_TO_SRV(sigsuspend,	PM),
	SCALL_TO_SRV(sprof,		PM),
	SCALL_TO_SRV(stat,		FS),
	SCALL_TO_SRV(stime,		PM),
	SCALL_TO_SRV(symlink,		FS),
	SCALL_TO_SRV(sync,		FS),	/* 70 */

	SCALL_TO_SRV(sysuname,		PM),
	SCALL_TO_SRV(time,		PM),
	SCALL_TO_SRV(times,		PM),
	SCALL_TO_SRV(truncate,		FS),
	SCALL_TO_SRV(umask,		FS),
	SCALL_TO_SRV(umount,		FS),
	SCALL_TO_SRV(unlink,		FS),
	SCALL_TO_SRV(utime,		FS),
	SCALL_TO_SRV(waitpid,		PM),
	SCALL_TO_SRV(wait,		PM),	/* 80 */
	SCALL_TO_SRV(write,		FS),
};
