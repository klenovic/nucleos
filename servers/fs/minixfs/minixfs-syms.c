/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include <nucleos/limits.h>
#include <nucleos/kipc.h>
#include <nucleos/vfsif.h>
#include <servers/mfs/const.h>
#include <servers/mfs/type.h>
#include <servers/mfs/inode.h>

/* Definition of minixfs symbols. */
struct inode inode[NR_INODES];

/* list of unused/free inodes */
struct unused_inodes_t unused_inodes;

/* inode hashtable */
struct inodelist hash_inodes[INODE_HASH_SIZE];

unsigned int inode_cache_hit;
unsigned int inode_cache_miss;

off_t rdahedpos;		/* position to read ahead */
struct inode *rdahed_inode;	/* pointer to inode to read ahead */

/* The following variables are used for returning results to the caller. */
int err_code;	/* temporary storage for error number */
int rdwt_err;	/* status of last disk i/o request */

int cch[NR_INODES];

kipc_msg_t fs_m_in;
kipc_msg_t fs_m_out;
int FS_STATE;
vfs_ucred_t credentials;

uid_t caller_uid;
gid_t caller_gid;

time_t boottime;	/* time in seconds at system boot */
int use_getuptime2;	/* Should be removed togetherwith boottime */

int req_nr;

int SELF_E;

struct inode *chroot_dir;

short path_processed;		/* number of characters processed */
char user_path[PATH_MAX+1];	/* pathname to be processed */
char *vfs_slink_storage;
int Xsymloop;

dev_t fs_dev;		/* The device that is handled by this FS_PROC_NR proc.
                                 */
char fs_dev_label[16];	/* Name of the device driver that is handled
			 * by this FS_PROC_NR proc.
			 */
int unmountdone;
int exitsignaled;

/* our block size. */
int fs_block_size;

/* Buffer cache. */
struct buf buf[NR_BUFS];
struct buf *buf_hash[NR_BUFS];	/* the buffer hash table */
