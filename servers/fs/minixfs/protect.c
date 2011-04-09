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
#include <nucleos/unistd.h>
#include <servers/mfs/buf.h>
#include <servers/mfs/inode.h>
#include <servers/mfs/super.h>
#include <nucleos/vfsif.h>

static in_group(gid_t grp);

/*===========================================================================*
 *				fs_chmod				     *
 *===========================================================================*/
int fs_chmod()
{
/* Perform the chmod(name, mode) system call. */

  register struct inode *rip;
  register int r;
  
  /* Temporarily open the file. */
  if ( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE) {
printk("MFS(%d) get_inode by fs_chmod() failed\n", SELF_E);
        return(-EINVAL);
  }

  /* Now make the change. Clear setgid bit if file is not in caller's grp */
  rip->i_mode = (rip->i_mode & ~ALL_MODES) | (fs_m_in.REQ_MODE & ALL_MODES);
  rip->i_update |= CTIME;
  rip->i_dirt = DIRTY;

  /* Return full new mode to caller. */
  fs_m_out.RES_MODE = rip->i_mode;

  put_inode(rip);
  return 0;
}


/*===========================================================================*
 *				fs_chown				     *
 *===========================================================================*/
int fs_chown()
{
  register struct inode *rip;
  register int r;
  /* Temporarily open the file. */
  caller_uid = fs_m_in.REQ_UID;
  caller_gid = fs_m_in.REQ_GID;
  
  /* Temporarily open the file. */
  if ( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE) {
printk("MFS(%d) get_inode by fs_chown() failed\n", SELF_E);
        return(-EINVAL);
  }

  /* Not permitted to change the owner of a file on a read-only file sys. */
  r = read_only(rip);
  if (r == 0) {
	rip->i_uid = fs_m_in.REQ_UID;
	rip->i_gid = fs_m_in.REQ_GID;
	rip->i_mode &= ~(I_SET_UID_BIT | I_SET_GID_BIT);
	rip->i_update |= CTIME;
	rip->i_dirt = DIRTY;
  }

  /* Update caller on current mode, as it may have changed. */
  fs_m_out.RES_MODE = rip->i_mode;

  put_inode(rip);

  return(r);
}

/*===========================================================================*
 *				fs_access_o				     *
 *===========================================================================*/
int fs_access_o()
{
  struct inode *rip;
  register int r;
  
  /* Temporarily open the file whose access is to be checked. */
  caller_uid = fs_m_in.REQ_UID;
  caller_gid = fs_m_in.REQ_GID;
  
  /* Temporarily open the file. */
  if ( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE) {
printk("MFS(%d) get_inode by fs_access() failed\n", SELF_E);
        return(-EINVAL);
  }

  /* Now check the permissions. */
  r = forbidden(rip, (mode_t) fs_m_in.REQ_MODE);
  put_inode(rip);
  return(r);
}

/*===========================================================================*
 *				forbidden				     *
 *===========================================================================*/
int forbidden(register struct inode *rip, mode_t access_desired)
{
/* Given a pointer to an inode, 'rip', and the access desired, determine
 * if the access is allowed, and if not why not.  The routine looks up the
 * caller's uid in the 'fproc' table.  If access is allowed, OK is returned
 * if it is forbidden, -EACCES is returned.
 */

  register struct inode *old_rip = rip;
  register struct minix3_super_block *sp;
  register mode_t bits, perm_bits;
  int r, shift, type;

  if (caller_uid == (uid_t)-1 && caller_gid == (uid_t)-1)
  {
	printk(
	"forbidden: warning caller_uid and caller_gid not initialized\n");
  }

  /*
  if (rip->i_mount == I_MOUNT)	
	for (sp = &super_block[1]; sp < &super_block[NR_SUPERS]; sp++)
		if (sp->s_imount == rip) {
			rip = get_inode(sp->s_dev, ROOT_INODE);
			break;
		} 
  */

  /* Isolate the relevant rwx bits from the mode. */
  bits = rip->i_mode;
  if (caller_uid == SU_UID) {
	/* Grant read and write permission.  Grant search permission for
	 * directories.  Grant execute permission (for non-directories) if
	 * and only if one of the 'X' bits is set.
	 */
	if ( (bits & I_TYPE) == I_DIRECTORY ||
	     bits & ((X_BIT << 6) | (X_BIT << 3) | X_BIT))
		perm_bits = R_BIT | W_BIT | X_BIT;
	else
		perm_bits = R_BIT | W_BIT;
  } else {
	if (caller_uid == rip->i_uid) shift = 6;	/* owner */
	else if (caller_gid == rip->i_gid ) shift = 3;	/* group */
	else shift = 0;					/* other */
	perm_bits = (bits >> shift) & (R_BIT | W_BIT | X_BIT);
  }

  /* If access desired is not a subset of what is allowed, it is refused. */
  r = 0;
  if ((perm_bits | access_desired) != perm_bits) r = -EACCES;

  /* Check to see if someone is trying to write on a file system that is
   * mounted read-only.
   */
  type = rip->i_mode & I_TYPE;
  if (r == 0)
	if (access_desired & W_BIT)
	 	r = read_only(rip);

#if 0
  if (r != 0) printk(
  "forbidden: caller uid/gid %d/%d object uid/gid %d/%d, returning %d\n",
	caller_uid, caller_gid, rip->i_uid, rip->i_gid, r);
#endif

  if (rip != old_rip) put_inode(rip);

  return(r);
}

/*===========================================================================*
 *				read_only				     *
 *===========================================================================*/
int read_only(ip)
struct inode *ip;		/* ptr to inode whose file sys is to be cked */
{
/* Check to see if the file system on which the inode 'ip' resides is mounted
 * read only.  If so, return -EROFS, else return OK.
 */

  register struct minix3_super_block *sp;

  sp = ip->i_sp;
  return(sp->s_rd_only ? -EROFS : 0);
}



