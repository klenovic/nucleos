/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */


#include "fs.h"
#include <nucleos/stat.h>
#include <nucleos/statfs.h>
#include <nucleos/com.h>
#include <nucleos/string.h>
#include <servers/mfs/buf.h>
#include <servers/mfs/inode.h>
#include <servers/mfs/super.h>
#include <nucleos/vfsif.h>

static int stat_inode(struct inode *rip, int who_e, cp_grant_id_t gid);

/**
 * Common code for stat and fstat system calls.
 * @param rip  pointer to inode to stat
 * @param who_e  caller endpoint
 * @param gid  grant for the stat buf
 * @return 0 on success
 */
static int stat_inode(struct inode *rip, int who_e, cp_grant_id_t gid)
{
	struct kstat ksb;
	mode_t mo;
	int r, s;

	/* Update the atime, ctime, and mtime fields in the inode, if need be. */
	if (rip->i_update)
		update_times(rip);

	/* Fill in the ksb struct. */
	mo = rip->i_mode & I_TYPE;

	/* true iff special */
	s = (mo == I_CHAR_SPECIAL || mo == I_BLOCK_SPECIAL);

	memset(&ksb, 0, sizeof(ksb));

	ksb.dev = rip->i_dev;
	ksb.ino = rip->i_num;
	ksb.mode = rip->i_mode;
	ksb.nlink = rip->i_nlinks;
	ksb.uid = rip->i_uid;
	ksb.gid = rip->i_gid;
	ksb.rdev = (dev_t)(s ? rip->i_zone[0] : NO_DEV);
	ksb.size = rip->i_size;
	ksb.atime.tv_sec = rip->i_atime;
	ksb.mtime.tv_sec = rip->i_mtime;
	ksb.ctime.tv_sec = rip->i_ctime;
	ksb.atime.tv_nsec = 0;
	ksb.mtime.tv_nsec = 0;
	ksb.ctime.tv_nsec = 0;

	/* @nucleos: NOT correct e.g. see minix_getattr() in linux sources fs/minix/inode.c */
	ksb.blksize = 1024;
	ksb.blocks = ((ksb.size + 511) >> 9);

	r = sys_safecopyto(who_e, gid, 0, (vir_bytes)&ksb, (phys_bytes)sizeof(ksb), D);

	return(r);
}


/*===========================================================================*
 *				fs_fstatfs				     *
 *===========================================================================*/
int fs_fstatfs(void)
{
	struct statfs st;
	struct inode *rip;
	int r;

	if((rip = find_inode(fs_dev, ROOT_INODE)) == NIL_INODE)
		return -EFAULT;

	st.f_bsize = rip->i_sp->s_block_size;

	/* Copy the struct to user space. */
	r = sys_safecopyto(fs_m_in.m_source, fs_m_in.REQ_GRANT, 0, (vir_bytes)&st,
			   (phys_bytes)sizeof(st), D);

	return(r);
}

/*===========================================================================*
 *                             fs_stat					     *
 *===========================================================================*/
int fs_stat(void)
{
	register int r;			/* return value */
	register struct inode *rip;	/* target inode */

	if ((rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE)
		return(-EINVAL);

	r = stat_inode(rip, fs_m_in.m_source, fs_m_in.REQ_GRANT);

	put_inode(rip);	/* release the inode */

	return(r);
}
