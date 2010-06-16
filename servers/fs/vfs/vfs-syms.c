/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include "fs.h"
#include "file.h"
#include <servers/vfs/fproc.h>
#include "lock.h"
#include "vnode.h"
#include "vmnt.h"

/* File System global variables */
struct fproc *fp;	/* pointer to caller's fproc struct */
int super_user;		/* 1 if caller is super_user, else 0 */
int susp_count;		/* number of procs suspended on pipe */
int nr_locks;		/* number of locks currently in place */
int reviving;		/* number of pipe processes to be revived */

dev_t root_dev;		/* device number of the root device */
int ROOT_FS_E;		/* kernel endpoint of the root FS proc */
int last_login_fs_e;	/* endpoint of the FS proc that logged in
			 *  before the corresponding mount request
			 */
u32_t system_hz;	/* system clock frequency. */

/* The parameters of the call are kept here. */
kipc_msg_t m_in;		/* the input message itself */
kipc_msg_t m_out;		/* the output message used for reply */
int who_p, who_e;	/* caller's proc number, endpoint */
int call_nr;		/* system call number */
kipc_msg_t mount_m_in;	/* the input message itself */

char user_fullpath[PATH_MAX+1];	/* storage for user path name */
short cum_path_processed;	/* number of characters processed */

/* The following variables are used for returning results to the caller. */
int err_code;		/* temporary storage for error number */
