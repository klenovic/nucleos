/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_GLO_H
#define __SERVERS_VFS_GLO_H

/* File System global variables */
extern struct fproc *fp;	/* pointer to caller's fproc struct */
extern int super_user;		/* 1 if caller is super_user, else 0 */
extern int susp_count;		/* number of procs suspended on pipe */
extern int nr_locks;		/* number of locks currently in place */
extern int reviving;		/* number of pipe processes to be revived */

extern dev_t root_dev;		/* device number of the root device */
extern int ROOT_FS_E;           /* kernel endpoint of the root FS_PROC_NR proc */
extern int last_login_fs_e;     /* endpoint of the FS_PROC_NR proc that logged in
                                   before the corresponding mount request */
extern u32_t system_hz;		/* system clock frequency. */

/* The parameters of the call are kept here. */
extern message m_in;		/* the input message itself */
extern message m_out;		/* the output message used for reply */
extern int who_p, who_e;	/* caller's proc number, endpoint */
extern int call_nr;		/* system call number */
extern message mount_m_in;	/* the input message itself */

extern char user_fullpath[PATH_MAX+1];    /* storage for user path name */
extern short cum_path_processed;        /* number of characters processed */

/* The following variables are used for returning results to the caller. */
extern int err_code;		/* temporary storage for error number */

/* Data initialized elsewhere. */
extern int (*call_vec[])(void); /* sys call table */
extern char dot1[2];   /* dot1 (&dot1[0]) and dot2 (&dot2[0]) have a special */
extern char dot2[3];   /* meaning to search_dir: no access permission check. */

#endif /* __SERVERS_VFS_GLO_H */
