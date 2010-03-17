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
	int (*fill_msg)(message *msg, const struct pt_regs *r);
};

static struct endpt_args scall_to_srv[NR_syscalls];

#define SCALL_TO_SRV(syscall, server) \
	[ __NNR_ ## syscall ] = { server ## _PROC_NR, msg_ ## syscall }

#define SCALL_TO_ANY(syscall, server) \
	[ __NNR_ ## syscall ] = { server, msg_ ## syscall }

static inline long strnlen_user(const char __user *s, size_t maxlen)
{
	char name[PATH_MAX];
	unsigned long len = (VM_STACKTOP - (unsigned long)s);

	/* We must not cross the top of stack during copy */
	len = (len < maxlen) ? len : maxlen;

	/* Just hang on for now
	 * @nucleos: add kernel oops here
	 */
	if (len > PATH_MAX)
		for(;;);

	copy_from_user(name, s, len);

	/* Return the length of string _including_ the NULL character */
	return strnlen(name, len) + 1;
}

endpoint_t map_scall_endpt(struct pt_regs *r)
{
	message kmsg;
	message __user *msg = (message*)r->ax;

	memset(&kmsg, 0, sizeof(message));

	/* copy the message from caller (user) space */
	copy_from_user(&kmsg, msg, sizeof(message));

	/* `m_type' contains syscall number */
	proc_ptr->syscall_0x80 = kmsg.m_type;

	/* @nucleos: The syscall numbers used by user (_NR_*) and kernel may be different.
	 *           E.g. user calls mmap via __NR_mmap but system knows it as VM_MMAP. In
	 *           this case the m_type can be changed to appropriate number in msg_* function.
	 *           Thing to change ???
	 */
	r->ax = kmsg.m_type;

	/* don't cross the range */
	if (r->ax >= NR_syscalls)
		goto err_nosys;

	/* fill the message according to passed arguments */
	if (scall_to_srv[r->ax].fill_msg)
		scall_to_srv[r->ax].fill_msg(&kmsg, r);
	else
		goto err_nosys;

	/* save all these regs and then retore them on exit (arch_finish_check) */
	proc_ptr->clobregs[CLOBB_REG_EBX] = r->bx;
	proc_ptr->clobregs[CLOBB_REG_ECX] = r->cx;
	proc_ptr->clobregs[CLOBB_REG_EDX] = r->dx;
	proc_ptr->clobregs[CLOBB_REG_ESI] = r->si;
	proc_ptr->clobregs[CLOBB_REG_EDI] = r->di;
	proc_ptr->clobregs[CLOBB_REG_EBP] = r->bp;

	/* sligh check whether the endpoint is valid */
	if (scall_to_srv[r->ax].endpt == ENDPT_ANY)
		goto err_nosys;

	/* @nucleos: This mapping _must_ change in the future. It is here because of current
	 *           kIPC. It requires to have message in caller's (user's) space.
	 */
	/* copy the filled message into caller (user) space */
	copy_to_user(msg, &kmsg, sizeof(message));

	r->ax = scall_to_srv[r->ax].endpt;
	r->bx = (unsigned long)msg;
	r->cx = KIPC_SENDREC;
	r->dx = 0;

	r->ax = sys_call(r->cx, r->ax, (message*)r->bx, r->dx);

	return r->ax;

err_nosys:
	proc_ptr->syscall_0x80 = 0;

	/* @nucleos: this is ignored and the syscall result is got from message */
	proc_ptr->p_reg.retreg = -ENOSYS;

	/* set errno and copy back to user */
	kmsg.m_type = -ENOSYS;
	copy_to_user(msg, &kmsg, sizeof(message));

	return -ENOSYS;
}

static int msg_access(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m3_i2 = r->cx;		/* mode */

	return 0;
}

static int msg_alarm(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* seconds */

	return 0;
}

static int msg_brk(message *msg, const struct pt_regs *r)
{
	msg->PMBRK_ADDR = (void*)r->bx;		/* addr */

	return 0;
}

static int msg_chdir(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	return 0;
}

static int msg_chmod(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m3_i2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_chown(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* name */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_i2 = (uid_t)r->cx;	/* owner */
	msg->m1_i3 = (gid_t)r->dx;	/* group */

	return 0;
}

static int msg_chroot(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* path */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	return 0;
}

static int msg_close(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;

	return 0;
}

static int msg_cprof(message *msg, const struct pt_regs *r)
{
	msg->PROF_ACTION	= r->bx;		/* action */
	msg->PROF_MEM_SIZE	= r->cx;		/* size */
	msg->PROF_CTL_PTR	= (void *)r->dx;	/* ctl_ptr */
	msg->PROF_MEM_PTR	= (void *)r->si;	/* mem_ptr */

	return 0;
}

static int msg_creat(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char*)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m3_i2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_dup(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_dup2(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */
	msg->m1_i2 = r->cx;	/* descriptor */

	return 0;
}

static int msg_exec(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* filename */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (char*)r->cx;	/* frame */
	msg->m1_i2 = r->dx;		/* frame_size */

	return 0;
}

static int msg_exit(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;

	return 0;
}

static int msg_fchdir(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_fchmod(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_i2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_fchown(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_i2 = (uid_t)r->cx;	/* owner */
	msg->m1_i3 = (gid_t)r->dx;	/* group */

	return 0;
}

static int msg_fcntl(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_i2 = r->cx;		/* cmd */
	msg->m1_i3 = r->dx;		/* flags */
	msg->m1_p1 = (void*)r->dx;	/* lock */

	return 0;
}

static int msg_fork(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_fstat(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */

	return 0;
}

static int msg_fstatfs(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */

	return 0;
}

static int msg_fsync(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* descriptor */

	return 0;
}

static int msg_ftruncate(message *msg, const struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l1 = (off_t)r->cx;	/* length */

	return 0;
}

static int msg_getdents(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* struct. dirent */
	msg->m1_i2 = (size_t)r->dx;		/* count */

	return 0;
}

static int msg_getegid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_geteuid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getgid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getgroups(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* size */
	msg->m1_p1 = (gid_t*)r->cx;	/* list[] */

	return 0;
}

static int msg_getitimer(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* which */
	msg->m1_p2 = (void*)r->cx;	/* value */

	return 0;
}

static int msg_getpgrp(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getpid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getppid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_getpriority(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* which */
	msg->m1_i2 = r->cx;	/* who */

	return 0;
}

static int msg_getsysinfo(message *msg, const struct pt_regs *r)
{
	/* In case of this syscall the endpoint is specify by user. */
	scall_to_srv[r->ax].endpt = r->bx;	/* who */

	msg->m1_i1 = r->cx;		/* what */
	msg->m1_p1 = (void*)r->dx;	/* where */

	return 0;
}

static int msg_getsysinfo_up(message *msg, const struct pt_regs *r)
{
	/* In case of this syscall the endpoint is specify by user. */
	scall_to_srv[r->ax].endpt = r->bx;	/* who */

	msg->SIU_WHAT = r->cx;		/* what */
	msg->SIU_WHERE = (void*)r->dx;	/* where */
	msg->SIU_LEN = r->si;		/* size */

	return 0;
}

static int msg_gettimeofday(message *msg, const struct pt_regs *r)
{
	msg->m2_p1 = (void*)r->bx;	/* timeval */
	msg->m2_l1 = r->cx;		/* timezone (expected NULL) */

	return 0;
}

static int msg_getuid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_ioctl(message *msg, const struct pt_regs *r)
{
	msg->TTY_LINE = r->bx;		/* descriptor */
	msg->TTY_REQUEST = r->cx;	/* request */
	msg->ADDRESS = (char *)r->dx;	/* data */

	return 0;
}

static int msg_kill(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (pid_t)r->bx;	/* pid */
	msg->m1_i2 = r->cx;	/* sig */

	return 0;
}

static int msg_link(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	msg->m1_p2 = (char*)r->cx;	/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX);

	return 0;
}

static int msg_llseek(message *msg, const struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l2 = r->cx;		/* offset (high) */
	msg->m2_l1 = r->dx;		/* offset (low) */
	msg->m2_p1 = (void*)r->si;	/* result (address) */
	msg->m2_i2 = r->di;		/* whence */

	return 0;
}

static int msg_lseek(message *msg, const struct pt_regs *r)
{
	msg->m2_i1 = r->bx;		/* descriptor */
	msg->m2_l1 = (off_t)r->cx;	/* offset */
	msg->m2_i2 = r->dx;		/* whence */

	return 0;
}

static int msg_lstat(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (char*)r->cx;	/* buffer */

	return 0;
}

static int msg_mkdir(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char*)r->bx;	/* name */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_i2 = (mode_t)r->cx;	/* mode */

	return 0;
}

static int msg_mknod(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *) r->bx;		/* pathname */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_i2 = (mode_t)r->cx;		/* mode */
	msg->m1_i3 = (dev_t)r->dx;		/* device */
	msg->m1_p2 = (char *) ((int) 0);	/* obsolete size field */

	return 0;
}

static int msg_mmap(message *msg, const struct pt_regs *r)
{
	struct mmap_arg_struct{
		unsigned long addr;
		unsigned long len;
		unsigned long prot;
		unsigned long flags;
		unsigned long fd;
		unsigned long offset;
	} parm;

	copy_from_user(&parm, (void*)r->bx, sizeof(struct mmap_arg_struct));

	msg->m_type = NNR_VM_MMAP;	/* VM syscall number */

	msg->VMM_ADDR = (vir_bytes)parm.addr;
	msg->VMM_LEN = (size_t)parm.len;
	msg->VMM_PROT = parm.prot;
	msg->VMM_FLAGS = parm.flags;
	msg->VMM_FD = parm.fd;
	msg->VMM_OFFSET = (off_t)parm.offset;

	return 0;
}

static int msg_mount(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* special */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	msg->m1_p2 = (char *)r->cx;	/* name */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX);

	msg->m1_i3 = r->dx;		/* mountflags */
	msg->m1_p3 = (char *)r->si;	/* ep */

	return 0;
}

static int msg_munmap(message *msg, const struct pt_regs *r)
{
	msg->m_type = NNR_VM_MUNMAP;	/* VM syscall number */

	msg->VMUM_ADDR = (void*)r->bx;
	msg->VMUM_LEN = (size_t)r->cx;

	return 0;
}

static int msg_munmap_text(message *msg, const struct pt_regs *r)
{
	msg->m_type = NNR_VM_MUNMAP_TEXT;	/* VM syscall number */

	msg->VMUM_ADDR = (void*)r->bx;
	msg->VMUM_LEN = (size_t)r->cx;

	return 0;
}

static int msg_open(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* pathname */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_i2 = r->cx;		/* flags */
	msg->m1_i3 = (mode_t)r->dx;	/* mode */

	return 0;
}

static int msg_pause(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_pipe(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* pointer to fild */

	return 0;
}

static int msg_ptrace(message *msg, const struct pt_regs *r){
	return 0;
}

static int msg_read(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (void*)r->cx;	/* buffer */
	msg->m1_i2 = r->dx;		/* count */

	return 0;
}

static int msg_readlink(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (void *)r->cx;	/* buffer */
	msg->m1_i2 = r->dx;		/* bufsiz */

	return 0;
}

static int msg_reboot(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* how */
	msg->m1_p1 = (void*)r->cx;	/* code */
	msg->m1_i2 = (size_t)r->dx	/* size of code */;

	return 0;
}

static int msg_rename(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (void *)r->cx;	/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX);

	return 0;
}

static int msg_rmdir(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	return 0;
}

static int msg_select(message *msg, const struct pt_regs *r)
{
	msg->SEL_NFDS = r->bx;			/* nfds */
	msg->SEL_READFDS = (char *) r->cx;	/* readfds */
	msg->SEL_WRITEFDS = (char *) r->dx;	/* writefds */
	msg->SEL_ERRORFDS = (char *) r->si;	/* errorfds */
	msg->SEL_TIMEOUT = (char *) r->di;	/* timeout */

	return 0;
}

static int msg_setegid(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (gid_t)r->bx;	/* gid */

	return 0;
}

static int msg_seteuid(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (uid_t)r->bx;	/* euid */

	return 0;
}

static int msg_setgid(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (gid_t)r->bx;	/* gid */

	return 0;
}

static int msg_setgroups(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* size */
	msg->m1_p1 = (gid_t*)r->cx;	/* *list */

	return 0;
}

static int msg_setitimer(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* which */
	msg->m1_p1 = (void*)r->cx;	/* value */
	msg->m1_p2 = (void*)r->dx;	/* ovalue */

	return 0;
}

static int msg_setpriority(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;	/* which */
	msg->m1_i2 = r->cx;	/* who */
	msg->m1_i3 = r->dx;	/* prio */

	return 0;
}

static int msg_setsid(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_setuid(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (uid_t)r->bx;	/* uid */

	return 0;
}

static int msg_sigaction(message *msg, const struct pt_regs *r)
{
	msg->m1_i2 = r->bx;		/* sig */
	msg->m1_p1 = (void*)r->cx;	/* act */
	msg->m1_p2 = (void*)r->dx;	/* oact */
	msg->m1_p3 = (void*)r->si;	/* __sigreturn */

	return 0;
}

static int msg_signal(message *msg, const struct pt_regs *r)
{
	/* n/a: implemented via sigaction */

	return 0;
}

static int msg_sigpending(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* set */

	return 0;
}

static int msg_sigprocmask(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* how */
	msg->m1_p1 = (void*)r->cx;	/* set */
	msg->m1_p2 = (void*)r->dx;	/* oldset */

	return 0;
}

static int msg_sigreturn(message *msg, const struct pt_regs *r)
{
	msg->m2_p1 = (void*)r->bx;	/* sigcontext */
	msg->m2_l1 = r->cx;		/* mask */
	msg->m2_i2 = r->dx;		/* flags */

	return 0;
}

static int msg_sigsuspend(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* set */

	return 0;
}

static int msg_sprof(message *msg, const struct pt_regs *r)
{
	msg->PROF_ACTION = r->bx;		/* action */
	msg->PROF_MEM_SIZE = r->cx;		/* size */
	msg->PROF_FREQ = r->dx;			/* freq */
	msg->PROF_CTL_PTR = (void*)r->si;	/* ctl_ptr */
	msg->PROF_MEM_PTR = (void*)r->di;	/* mem_ptr */

	return 0;
}

static int msg_stat(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;	/* path */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (void *)r->cx;	/* buffer */

	return 0;
}

static int msg_stime(message *msg, const struct pt_regs *r)
{
	msg->m2_p1 = (void*)r->bx;	/* time */

	return 0;
}

static int msg_svrctl(message *msg, const struct pt_regs *r)
{
	return 0;
}

static int msg_symlink(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (char *)r->bx;		/* oldpath */
	msg->m1_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m1_p2 = (char *)r->cx;		/* newpath */
	msg->m1_i2 = strnlen_user((char *)r->cx, PATH_MAX);

	return 0;
}

static int msg_sync(message *msg, const struct pt_regs *r)
{
	/* no args */

	return 0;
}

static int msg_uname(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* utsname */

	return 0;
}

static int msg_time(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* time */

	return 0;
}

static int msg_times(message *msg, const struct pt_regs *r)
{
	msg->m1_p1 = (void*)r->bx;	/* times */

	return 0;
}

static int msg_truncate(message *msg, const struct pt_regs *r)
{
	msg->m2_p1 = (char *)r->bx;		/* path */
	msg->m2_i1 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m2_l1 = (off_t)r->cx;		/* length */

	return 0;
}

static int msg_umask(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = (mode_t)r->bx;	/* mask */

	return 0;
}

static int msg_umount(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* target */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	return 0;
}

static int msg_unlink(message *msg, const struct pt_regs *r)
{
	msg->m3_p1 = (char *)r->bx;	/* pathname */
	msg->m3_i1 = strnlen_user((char *)r->bx, PATH_MAX);

	return 0;
}

static int msg_utime(message *msg, const struct pt_regs *r)
{
	msg->m2_p1 = (char *)r->bx;	/* filename */
	msg->m2_i2 = strnlen_user((char *)r->bx, PATH_MAX);
	msg->m2_l1 = r->cx;		/* times pointer */

	return 0;
}

static int msg_wait(message *msg, const struct pt_regs *r)
{
	/* @nucleos: The `msg->m1_p1' is not really used but
	 *           `r->bx' is important. It holds the status
	 *           address.
	 */
	msg->m1_p1 = (void*)r->bx;	/* status */

	return 0;
}

static int msg_waitpid(message *msg, const struct pt_regs *r)
{
	/* @nucleos: The `msg->m1_p1' is not really used but
	 *           `r->cx' is important. It holds the status
	 *           address.
	 */
	msg->m1_i1 = r->bx;		/* pid */
	msg->m1_p1 = (void*)r->cx;	/* status */
	msg->m1_i2 = r->dx;		/* options */

	return 0;
}

static int msg_write(message *msg, const struct pt_regs *r)
{
	msg->m1_i1 = r->bx;		/* descriptor */
	msg->m1_p1 = (char *)r->cx;	/* buffer */
	msg->m1_i2 = (size_t)r->dx;	/* count */

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
