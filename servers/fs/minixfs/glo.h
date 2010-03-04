/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/vfsif.h>

extern off_t rdahedpos;		/* position to read ahead */
extern struct inode *rdahed_inode;	/* pointer to inode to read ahead */

/* The following variables are used for returning results to the caller. */
extern int err_code;		/* temporary storage for error number */
extern int rdwt_err;		/* status of last disk i/o request */

extern int cch[NR_INODES];

extern char dot1[2];   /* dot1 (&dot1[0]) and dot2 (&dot2[0]) have a special */
extern char dot2[3];   /* meaning to search_dir: no access permission check. */

extern int (*fs_call_vec[])(void); /* fs call table */

extern message fs_m_in;
extern message fs_m_out;
extern int FS_STATE;
extern vfs_ucred_t credentials;

extern uid_t caller_uid;
extern gid_t caller_gid;

extern time_t boottime;		/* time in seconds at system boot */
extern int use_getuptime2;	/* Should be removed togetherwith boottime */

extern int req_nr;

extern int SELF_E;

extern struct inode *chroot_dir;

extern short path_processed;      /* number of characters processed */
extern char user_path[PATH_MAX+1];  /* pathname to be processed */
extern char *vfs_slink_storage;
extern int Xsymloop;

extern dev_t fs_dev;    	/* The device that is handled by this FS_PROC_NR proc.
				 */
extern char fs_dev_label[16];	/* Name of the device driver that is handled
				 * by this FS_PROC_NR proc.
				 */
extern int unmountdone;
extern int exitsignaled;

/* our block size. */
extern int fs_block_size;

/* Buffer cache. */
extern struct buf buf[NR_BUFS];
extern struct buf *buf_hash[NR_BUFS];   /* the buffer hash table */

