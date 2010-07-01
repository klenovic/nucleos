/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <nucleos/uaccess.h>
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
 *           The exceptions are the getsysinfo[_up] and svrctl system calls. They might be replaced
 *           by /proc/sys in the future.
 *
 *           Maybe it make sense to create separate server<->kernel<->driver (internal)
 *           communication and separate libc<->kernel (external) communication.
 *           It allows:
 *               - to handle "pure" user (C library) request independently from the system. The
 *                 kIPC implemenentation doesn't have to change.
 *               - the things regarding messages (kipc_msg_t format, passing, dispatching, ...) to
 *                 be handled differently for C library.
 *               - the current implementation to be less affected.
 */

struct endpt_args {
	endpoint_t endpt;
	int (*fill_msg)(kipc_msg_t *msg, const struct pt_regs *r);
};

static struct endpt_args scall_to_srv[NR_syscalls];

#define SCALL_TO_SRV(syscall, server) \
	[ __NR_ ## syscall ] = { server ## _PROC_NR, msg_ ## syscall }

#define SCALL_TO_ANY(syscall, server) \
	[ __NR_ ## syscall ] = { server, msg_ ## syscall }

endpoint_t map_scall_endpt(struct pt_regs *r)
{
	kipc_msg_t *kmsg = &proc_ptr->p_sendmsg;
	int err = 0;

	/* `m_type' contains syscall number */
	proc_ptr->syscall_0x80 = r->ax;

	/* @nucleos: The syscall numbers used by user (_NR_*) and kernel may be different.
	 *           E.g. user calls mmap via __NR_mmap but system knows it as VM_MMAP. In
	 *           this case the m_type can be changed to appropriate number in msg_* function.
	 *           Thing to change ???
	 */
	kmsg->m_type = r->ax;

	/* don't cross the range */
	if (!r->ax || r->ax >= NR_syscalls) {
		err = -ENOSYS;
		goto fail_msg;
	}

	if (!scall_to_srv[r->ax].fill_msg) {
		err = -ENOSYS;
		goto fail_msg;
	}

	/* fill the message according to passed arguments */
	if ((err = scall_to_srv[r->ax].fill_msg(kmsg, r)) < 0)
		goto fail_msg;

	if (r->ax != __NR_sigreturn) {
		/* save all these regs and then retore them on kernel exit */
		proc_ptr->clobregs[CLOBB_REG_EBX] = r->bx;
		proc_ptr->clobregs[CLOBB_REG_ECX] = r->cx;
		proc_ptr->clobregs[CLOBB_REG_EDX] = r->dx;
		proc_ptr->clobregs[CLOBB_REG_ESI] = r->si;
		proc_ptr->clobregs[CLOBB_REG_EDI] = r->di;
		proc_ptr->clobregs[CLOBB_REG_EBP] = r->bp;
	}

	/* sligh check whether the endpoint is valid */
	if (scall_to_srv[r->ax].endpt == ENDPT_ANY) {
		err = -EFAULT;
		goto fail_msg;
	}

	r->ax = scall_to_srv[r->ax].endpt;
	r->bx = (unsigned long)kmsg;
	r->cx = KIPC_SENDREC;
	r->dx = 0;

	r->ax = kipc_call(r->cx, r->ax, (kipc_msg_t*)r->bx, r->dx);

	return r->ax;

fail_msg:
	proc_ptr->syscall_0x80 = 0;
	proc_ptr->p_reg.retreg = err;

	return err;
}

static int msg_access(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* pathname */
	msg->m_data2 = r->cx;	/* mode */

	return 0;
}

static int msg_alarm(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* seconds */

	return 0;
}

static int msg_brk(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->PMBRK_ADDR = r->bx;	/* addr */

	return 0;
}

static int msg_chdir(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* pathname */

	return 0;
}

static int msg_chmod(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;		/* pathname */
	msg->m_data2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_chown(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;		/* name */
	msg->m_data2 = (uid_t)r->cx;	/* owner */
	msg->m_data3 = (gid_t)r->dx;	/* group */

	return 0;
}

static int msg_chroot(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* path */

	return 0;
}

static int msg_close(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;

	return 0;
}

static int msg_cprof(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->PROF_ACTION = r->bx;	/* action */
	msg->PROF_MEM_SIZE = r->cx;	/* size */
	msg->PROF_CTL_PTR = r->dx;	/* ctl_ptr */
	msg->PROF_MEM_PTR = r->si;	/* mem_ptr */

	return 0;
}

static int msg_creat(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;		/* pathname */
	msg->m_data2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_dup(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_dup2(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data2 = r->cx;	/* descriptor */

	return 0;
}

static int msg_exec(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* filename */
	msg->m_data5 = r->cx;	/* frame */
	msg->m_data2 = r->dx;	/* frame_size */

	return 0;
}

static int msg_exit(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;

	return 0;
}

static int msg_fchdir(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_fchmod(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_fchown(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data2 = (uid_t)r->cx;	/* owner */
	msg->m_data3 = (gid_t)r->dx;	/* group */

	return 0;
}

static int msg_fcntl(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data2 = r->cx;	/* cmd */
	msg->m_data3 = r->dx;	/* flags */
	msg->m_data4 = r->dx;	/* lock */

	return 0;
}

static int msg_fork(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_fstat(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data4 = r->cx;	/* buffer */

	return 0;
}

static int msg_fstatfs(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data4 = r->cx;	/* buffer */

	return 0;
}

static int msg_fsync(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_ftruncate(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data4 = (off_t)r->cx;	/* length */

	return 0;
}

static int msg_getdents(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data4 = r->cx;		/* struct. dirent */
	msg->m_data2 = (size_t)r->dx;	/* count */

	return 0;
}

static int msg_getegid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_geteuid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getgid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getgroups(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* size */
	msg->m_data4 = r->cx;	/* list[] */

	return 0;
}

static int msg_getitimer(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* which */
	msg->m_data5 = r->cx;	/* value */

	return 0;
}

static int msg_getpgrp(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getpid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getppid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getpriority(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* which */
	msg->m_data2 = r->cx;	/* who */

	return 0;
}

static int msg_getsysinfo(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* In case of this syscall the endpoint is specify by user. */
	scall_to_srv[r->ax].endpt = r->bx;	/* who */

	msg->m_data1 = r->cx;	/* what */
	msg->m_data4 = r->dx;	/* where */

	return 0;
}

static int msg_getsysinfo_up(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* In case of this syscall the endpoint is specify by user. */
	scall_to_srv[r->ax].endpt = r->bx;	/* who */

	msg->SIU_WHAT = r->cx;	/* what */
	msg->SIU_WHERE = r->dx;	/* where */
	msg->SIU_LEN = r->si;	/* size */

	return 0;
}

static int msg_gettimeofday(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data6 = r->bx;	/* timeval */
	msg->m_data4 = r->cx;	/* timezone (expected NULL) */

	return 0;
}

static int msg_getuid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_ioctl(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->TTY_LINE = r->bx;		/* descriptor */
	msg->TTY_REQUEST = r->cx;	/* request */
	msg->ADDRESS = r->dx;		/* data */

	return 0;
}

static int msg_kill(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (pid_t)r->bx;	/* pid */
	msg->m_data2 = r->cx;		/* sig */

	return 0;
}

static int msg_link(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* oldpath */
	msg->m_data5 = r->cx;	/* newpath */

	return 0;
}

static int msg_llseek(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data5 = r->cx;	/* offset (high) */
	msg->m_data4 = r->dx;	/* offset (low) */
	msg->m_data6 = r->si;	/* result (address) */
	msg->m_data2 = r->di;	/* whence */

	return 0;
}

static int msg_lseek(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data4 = (off_t)r->cx;	/* offset */
	msg->m_data2 = r->dx;		/* whence */

	return 0;
}

static int msg_lstat(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* path */
	msg->m_data5 = r->cx;	/* buffer */

	return 0;
}

static int msg_mkdir(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;		/* name */
	msg->m_data2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_mknod(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;		/* pathname */
	msg->m_data2 = (mode_t)r->cx;	/* mode */
	msg->m_data3 = (dev_t)r->dx;	/* device */
	msg->m_data5 = 0;		/* obsolete size field */

	return 0;
}

static int msg_mmap(kipc_msg_t *msg, const struct pt_regs *r)
{
	struct mmap_arg_struct{
		unsigned long addr;
		unsigned long len;
		unsigned long prot;
		unsigned long flags;
		unsigned long fd;
		unsigned long offset;
	} parm;

	if (copy_from_user(&parm, (void*)r->bx, sizeof(struct mmap_arg_struct)))
		return -EFAULT;

	msg->m_type = NNR_VM_MMAP;	/* VM syscall number */

	msg->VMM_ADDR = (vir_bytes)parm.addr;
	msg->VMM_LEN = (size_t)parm.len;
	msg->VMM_PROT = parm.prot;
	msg->VMM_FLAGS = parm.flags;
	msg->VMM_FD = parm.fd;
	msg->VMM_OFFSET = (off_t)parm.offset;

	return 0;
}

static int msg_mount(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* special */
	msg->m_data5 = r->cx;	/* name */
	msg->m_data3 = r->dx;	/* mountflags */
	msg->m_data6 = r->si;	/* ep */

	return 0;
}

static int msg_munmap(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_type = NNR_VM_MUNMAP;	/* VM syscall number */

	msg->VMUM_ADDR = r->bx;
	msg->VMUM_LEN = (size_t)r->cx;

	return 0;
}

static int msg_munmap_text(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_type = NNR_VM_MUNMAP_TEXT;	/* VM syscall number */
	msg->VMUM_ADDR = r->bx;
	msg->VMUM_LEN = (size_t)r->cx;

	return 0;
}

static int msg_open(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;		/* pathname */
	msg->m_data2 = r->cx;		/* flags */
	msg->m_data3 = (mode_t)r->dx;	/* mode */

	return 0;
}

static int msg_pause(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_pipe(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* pointer to fild */

	return 0;
}

static int msg_ptrace(kipc_msg_t *msg, const struct pt_regs *r){

	msg->m_data2 = r->bx;		/* req */
	msg->m_data1 = r->cx;		/* pid */
	msg->PMTRACE_ADDR = r->dx;	/* addr */
	msg->m_data5 = r->si;		/* data */

	return 0;
}

static int msg_read(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* descriptor */
	msg->m_data4 = r->cx;	/* buffer */
	msg->m_data2 = r->dx;	/* count */

	return 0;
}

static int msg_readlink(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* path */
	msg->m_data5 = r->cx;	/* buffer */
	msg->m_data2 = r->dx;	/* bufsiz */

	return 0;
}

static int msg_reboot(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* how */
	msg->m_data4 = r->cx;		/* code */
	msg->m_data2 = (size_t)r->dx	/* size of code */;

	return 0;
}

static int msg_rename(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* oldpath */
	msg->m_data5 = r->cx;	/* newpath */

	return 0;
}

static int msg_rmdir(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* pathname */

	return 0;
}

static int msg_select(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->SEL_NFDS = r->bx;		/* nfds */
	msg->SEL_READFDS = r->cx;	/* readfds */
	msg->SEL_WRITEFDS = r->dx;	/* writefds */
	msg->SEL_ERRORFDS = r->si;	/* errorfds */
	msg->SEL_TIMEOUT = r->di;	/* timeout */

	return 0;
}

static int msg_setegid(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (gid_t)r->bx;	/* gid */

	return 0;
}

static int msg_seteuid(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (uid_t)r->bx;	/* euid */

	return 0;
}

static int msg_setgid(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (gid_t)r->bx;	/* gid */

	return 0;
}

static int msg_setgroups(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* size */
	msg->m_data4 = r->cx;	/* *list */

	return 0;
}

static int msg_setitimer(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* which */
	msg->m_data4 = r->cx;	/* value */
	msg->m_data5 = r->dx;	/* ovalue */

	return 0;
}

static int msg_setpriority(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* which */
	msg->m_data2 = r->cx;	/* who */
	msg->m_data3 = r->dx;	/* prio */

	return 0;
}

static int msg_setsid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_setuid(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (uid_t)r->bx;	/* uid */

	return 0;
}

static int msg_sigaction(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data2 = r->bx;	/* sig */
	msg->m_data4 = r->cx;	/* act */
	msg->m_data5 = r->dx;	/* oact */
	msg->m_data6 = r->si;	/* __sigreturn */

	return 0;
}

static int msg_signal(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* n/a: implemented via sigaction */

	return 0;
}

static int msg_sigpending(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* set */

	return 0;
}

static int msg_sigprocmask(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;	/* how */
	msg->m_data4 = r->cx;	/* set */
	msg->m_data5 = r->dx;	/* oldset */

	return 0;
}

static int msg_sigreturn(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data6 = r->bx;	/* sigcontext */
	msg->m_data4 = r->cx;	/* mask */
	msg->m_data2 = r->dx;	/* flags */

	return 0;
}

static int msg_sigsuspend(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* set */

	return 0;
}

static int msg_sprof(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->PROF_ACTION = r->bx;	/* action */
	msg->PROF_MEM_SIZE = r->cx;	/* size */
	msg->PROF_FREQ = r->dx;		/* freq */
	msg->PROF_CTL_PTR = r->si;	/* ctl_ptr */
	msg->PROF_MEM_PTR = r->di;	/* mem_ptr */

	return 0;
}

static int msg_stat(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* path */
	msg->m_data5 = r->cx;	/* buffer */

	return 0;
}

static int msg_stime(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data6 = r->bx;	/* time */

	return 0;
}

static int msg_svrctl(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* In case of this syscall the endpoint is specify by user. */
	scall_to_srv[r->ax].endpt = r->bx;	/* who */

	msg->m_data1 = r->cx;	/* request */
	msg->m_data6 = r->dx;	/* argp */
	return 0;
}

static int msg_symlink(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* oldpath */
	msg->m_data5 = r->cx;	/* newpath */

	return 0;
}

static int msg_sync(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_uname(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* utsname */

	return 0;
}

static int msg_time(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* time */

	return 0;
}

static int msg_times(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data4 = r->bx;	/* times */

	return 0;
}

static int msg_truncate(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data6 = r->bx;		/* path */
	msg->m_data4 = (off_t)r->cx;	/* length */

	return 0;
}

static int msg_umask(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = (mode_t)r->bx;	/* mask */

	return 0;
}

static int msg_umount(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* target */

	return 0;
}

static int msg_unlink(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data3 = r->bx;	/* pathname */

	return 0;
}

static int msg_utime(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data6 = r->bx;	/* filename */
	msg->m_data4 = r->cx;	/* times pointer */

	return 0;
}

static int msg_wait(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* @nucleos: The `msg->m_data4' is not really used but
	 *           `r->bx' is important. It holds the status
	 *           address.
	 */
	msg->m_data4 = r->bx;	/* status */

	return 0;
}

static int msg_waitpid(kipc_msg_t *msg, const struct pt_regs *r)
{
	/* @nucleos: The `msg->m_data4' is not really used but
	 *           `r->cx' is important. It holds the status
	 *           address.
	 */
	msg->m_data1 = r->bx;		/* pid */
	msg->m_data4 = r->cx;		/* status */
	msg->m_data2 = r->dx;		/* options */

	return 0;
}

static int msg_write(kipc_msg_t *msg, const struct pt_regs *r)
{
	msg->m_data1 = r->bx;		/* descriptor */
	msg->m_data4 = r->cx;		/* buffer */
	msg->m_data2 = (size_t)r->dx;	/* count */

	return 0;
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
	SCALL_TO_SRV(geteuid,		PM),
	SCALL_TO_SRV(getgid,		PM),
	SCALL_TO_SRV(getgroups,		PM),
	SCALL_TO_SRV(getitimer,		PM),
	SCALL_TO_SRV(getpgrp,		PM),	/* 30 */

	SCALL_TO_SRV(getpid,		PM),
	SCALL_TO_SRV(getppid,		PM),
	SCALL_TO_SRV(getpriority,	PM),
	SCALL_TO_ANY(getsysinfo,	ENDPT_ANY),
	SCALL_TO_ANY(getsysinfo_up,	ENDPT_ANY),
	SCALL_TO_SRV(gettimeofday,	PM),
	SCALL_TO_SRV(getuid,		PM),
	SCALL_TO_SRV(ioctl,		FS),
	SCALL_TO_SRV(kill,		PM),
	SCALL_TO_SRV(link,		FS),	/* 40 */

	SCALL_TO_SRV(llseek,		FS),
	SCALL_TO_SRV(lseek,		FS),
	SCALL_TO_SRV(lstat,		FS),
	SCALL_TO_SRV(mkdir,		FS),
	SCALL_TO_SRV(mknod,		FS),
	SCALL_TO_SRV(mmap,		VM),
	SCALL_TO_SRV(mount,		FS),
	SCALL_TO_SRV(munmap,		VM),
	SCALL_TO_SRV(munmap_text,	VM),
	SCALL_TO_SRV(open,		FS),	/* 50 */

	SCALL_TO_SRV(pause,		PM),
	SCALL_TO_SRV(pipe,		FS),
	SCALL_TO_SRV(ptrace,		PM),
	SCALL_TO_SRV(read,		FS),
	SCALL_TO_SRV(readlink,		FS),
	SCALL_TO_SRV(reboot,		PM),
	SCALL_TO_SRV(rename,		FS),
	SCALL_TO_SRV(rmdir,		FS),
	SCALL_TO_SRV(select,		FS),
	SCALL_TO_SRV(setegid,		PM),	/* 60 */

	SCALL_TO_SRV(seteuid,		PM),
	SCALL_TO_SRV(setgid,		PM),
	SCALL_TO_SRV(setgroups,		PM),
	SCALL_TO_SRV(setitimer,		PM),
	SCALL_TO_SRV(setpriority,	PM),
	SCALL_TO_SRV(setsid,		PM),
	SCALL_TO_SRV(setuid,		PM),
	SCALL_TO_SRV(sigaction,		PM),
	SCALL_TO_SRV(signal,		PM),
	SCALL_TO_SRV(sigpending,	PM),	/* 70 */

	SCALL_TO_SRV(sigprocmask,	PM),
	SCALL_TO_SRV(sigreturn,		PM),
	SCALL_TO_SRV(sigsuspend,	PM),
	SCALL_TO_SRV(sprof,		PM),
	SCALL_TO_SRV(stat,		FS),
	SCALL_TO_SRV(stime,		PM),
	SCALL_TO_ANY(svrctl,		ENDPT_ANY),
	SCALL_TO_SRV(symlink,		FS),
	SCALL_TO_SRV(sync,		FS),
	SCALL_TO_SRV(uname,		PM),	/* 80 */

	SCALL_TO_SRV(time,		PM),
	SCALL_TO_SRV(times,		PM),
	SCALL_TO_SRV(truncate,		FS),
	SCALL_TO_SRV(umask,		FS),
	SCALL_TO_SRV(umount,		FS),
	SCALL_TO_SRV(unlink,		FS),
	SCALL_TO_SRV(utime,		FS),
	SCALL_TO_SRV(wait,		PM),
	SCALL_TO_SRV(waitpid,		PM),
	SCALL_TO_SRV(write,		FS),	/* 90 */
};
