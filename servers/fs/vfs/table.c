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
#include <servers/vfs/fproc.h>
#include "lock.h"
#include "vnode.h"
#include "vmnt.h"

#define SCALL_HANDLER(syscall, handler) \
	[ __NR_ ## syscall ] = handler

int (*call_vec[])(void) = {
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
	SCALL_HANDLER(ftruncate,	do_ftruncate),
	SCALL_HANDLER(getdents,		do_getdents),
	SCALL_HANDLER(getsysinfo,	do_getsysinfo),
	SCALL_HANDLER(ioctl,		do_ioctl),
	SCALL_HANDLER(link,		do_link),
	SCALL_HANDLER(llseek,		scall_llseek),
	SCALL_HANDLER(lseek,		scall_lseek),
	SCALL_HANDLER(lstat,		do_lstat),
	SCALL_HANDLER(mkdir,		do_mkdir),
	SCALL_HANDLER(mknod,		do_mknod),
	SCALL_HANDLER(mount,		do_mount),
	SCALL_HANDLER(open,		scall_open),
	SCALL_HANDLER(pipe,		scall_pipe),
	SCALL_HANDLER(read,		do_read),
	SCALL_HANDLER(readlink,		do_readlink),
	SCALL_HANDLER(rename,		do_rename),
	SCALL_HANDLER(rmdir,		scall_rmdir),
	SCALL_HANDLER(select,		do_select),
	SCALL_HANDLER(stat,		do_stat),
	SCALL_HANDLER(svrctl,		do_svrctl),
	SCALL_HANDLER(symlink,		scall_symlink),
	SCALL_HANDLER(sync,		do_sync),
	SCALL_HANDLER(truncate,		do_truncate),
	SCALL_HANDLER(umask,		do_umask),
	SCALL_HANDLER(umount,		do_umount),
	SCALL_HANDLER(unlink,		do_unlink),
	SCALL_HANDLER(utime,		scall_utime),
	SCALL_HANDLER(write,		do_write),

	[KCNR_FS_READY]		= do_fsready,		/* 57 = FS proc login */
};

/* This should not fail with "array size is negative": */
extern int dummy[sizeof(call_vec) <= NR_syscalls * sizeof(call_vec[0]) ? 1 : -1];
