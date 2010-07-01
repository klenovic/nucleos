/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

static int stat_inode(struct inode *rip, int who_e,
				     cp_grant_id_t gid);


/*===========================================================================*
 *				stat_inode				     *
 *===========================================================================*/
static int stat_inode(rip, who_e, gid)
register struct inode *rip;	/* pointer to inode to stat */
int who_e;			/* Caller endpoint */
cp_grant_id_t gid;		/* grant for the stat buf */
{
/* Common code for stat and fstat system calls. */

  struct stat statbuf;
  mode_t mo;
  int r, s;

  /* Update the atime, ctime, and mtime fields in the inode, if need be. */
  if (rip->i_update) update_times(rip);

  /* Fill in the statbuf struct. */
  mo = rip->i_mode & I_TYPE;

  /* true iff special */
  s = (mo == I_CHAR_SPECIAL || mo == I_BLOCK_SPECIAL);

  statbuf.st_dev = rip->i_dev;
  statbuf.st_ino = rip->i_num;
  statbuf.st_mode = rip->i_mode;
  statbuf.st_nlink = rip->i_nlinks;
  statbuf.st_uid = rip->i_uid;
  statbuf.st_gid = rip->i_gid;
  statbuf.st_rdev = (dev_t) (s ? rip->i_zone[0] : NO_DEV);
  statbuf.st_size = rip->i_size;
  statbuf.st_atime = rip->i_atime;
  statbuf.st_mtime = rip->i_mtime;
  statbuf.st_ctime = rip->i_ctime;

  /* Copy the struct to user space. */
  r = sys_safecopyto(who_e, gid, 0, (vir_bytes) &statbuf,
  		(phys_bytes) sizeof(statbuf), D);
  
  return(r);
}


/*===========================================================================*
 *				fs_fstatfs				     *
 *===========================================================================*/
int fs_fstatfs()
{
  struct statfs st;
  struct inode *rip;
  int r;

  if((rip = find_inode(fs_dev, ROOT_INODE)) == NIL_INODE)
	  return(-EINVAL);
  
  st.f_bsize = rip->i_sp->s_block_size;
  
  /* Copy the struct to user space. */
  r = sys_safecopyto(fs_m_in.m_source, fs_m_in.REQ_GRANT, 0, (vir_bytes) &st,
		     (phys_bytes) sizeof(st), D);
  
  return(r);
}

/*===========================================================================*
 *                             fs_stat					     *
 *===========================================================================*/
int fs_stat()
{
  register int r;              /* return value */
  register struct inode *rip;  /* target inode */

  if ((rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE)
        return(-EINVAL);
  
  r = stat_inode(rip, fs_m_in.m_source, fs_m_in.REQ_GRANT);
  put_inode(rip);		/* release the inode */
  return(r);
}



