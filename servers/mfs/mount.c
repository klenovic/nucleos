/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */


#include "fs.h"
#include <nucleos/fcntl.h>
#include <nucleos/string.h>
#include <nucleos/com.h>
#include <nucleos/stat.h>
#include "buf.h"
#include "inode.h"
#include "super.h"
#include "drivers.h"
#include <servers/ds/ds.h>
#include <nucleos/vfsif.h>

struct super_block superblock;

/*===========================================================================*
 *				fs_readsuper_s				     *
 *===========================================================================*/
int fs_readsuper_s()
{
/* This function reads the superblock of the partition, gets the root inode
 * and sends back the details of them. Note, that the FS_PROC_NR process does not
 * know the index of the vmnt object which refers to it, whenever the pathname 
 * lookup leaves a partition an ELEAVEMOUNT error is transferred back 
 * so that the VFS knows that it has to find the vnode on which this FS_PROC_NR 
 * process' partition is mounted on.
 */
  struct super_block *xp;
  struct inode *root_ip;
  cp_grant_id_t label_gid;
  size_t label_len;
  int r = 0;
  unsigned long tasknr;
  endpoint_t driver_e;

  fs_dev = fs_m_in.REQ_DEV;

  label_gid= fs_m_in.REQ_GRANT2;
  label_len= fs_m_in.REQ_PATH_LEN;

  if (label_len > sizeof(fs_dev_label))
  {
	printf("mfs:fs_readsuper: label too long\n");
	return -EINVAL;
  }

  r= sys_safecopyfrom(fs_m_in.m_source, label_gid, 0, (vir_bytes)fs_dev_label,
	label_len, D);
  if (r != 0)
  {
	printf("mfs:fs_readsuper: safecopyfrom failed: %d\n", r);
	return -EINVAL;
  }

  r= ds_retrieve_u32(fs_dev_label, (u32_t*)&tasknr);
  if (r != 0)
  {
	printf("mfs:fs_readsuper: ds_retrieve_u32 failed for '%s': %d\n",
		fs_dev_label, r);
	return -EINVAL;
  }

  driver_e= tasknr;

  /* Map the driver endpoint for this major */
  driver_endpoints[(fs_dev >> MAJOR) & BYTE].driver_e =  driver_e;
  use_getuptime2= TRUE;				/* Should be removed with old
						 * getuptime call.
						 */
  vfs_slink_storage = (char *)0xdeadbeef;	/* Should be removed together
						 * with old lookup code.
						 */;

  /* Open the device the file system lives on. */
  if (dev_open(driver_e, fs_dev, driver_e,
	fs_m_in.REQ_READONLY ? R_BIT : (R_BIT|W_BIT)) != 0) {
        return(-EINVAL);
  }
  
  /* Fill in the super block. */
  superblock.s_dev = fs_dev;	/* read_super() needs to know which dev */
  r = read_super(&superblock);

  /* Is it recognized as a Minix filesystem? */
  if (r != 0) {
	superblock.s_dev = NO_DEV;
  	dev_close(driver_e, fs_dev);
	return(r);
  }

  set_blocksize(superblock.s_block_size);
  
  /* Get the root inode of the mounted file system. */
  if ( (root_ip = get_inode(fs_dev, ROOT_INODE)) == NIL_INODE)  {
	printf("MFS: couldn't get root inode?!\n");
	superblock.s_dev = NO_DEV;
  	dev_close(driver_e, fs_dev);
	return -EINVAL;
  }
  
  if (root_ip != NIL_INODE && root_ip->i_mode == 0) {
	printf("MFS: zero mode for root inode?!\n");
        put_inode(root_ip);
	superblock.s_dev = NO_DEV;
  	dev_close(driver_e, fs_dev);
  	return -EINVAL;
  }

  superblock.s_rd_only = fs_m_in.REQ_READONLY;
  superblock.s_is_root = fs_m_in.REQ_ISROOT;
  
  /* Root inode properties */
  fs_m_out.RES_INODE_NR = root_ip->i_num;
  fs_m_out.RES_MODE = root_ip->i_mode;
  fs_m_out.RES_FILE_SIZE = root_ip->i_size;
  fs_m_out.RES_UID = root_ip->i_uid;
  fs_m_out.RES_GID = root_ip->i_gid;

  return r;
}


/*===========================================================================*
 *				fs_readsuper_o				     *
 *===========================================================================*/
int fs_readsuper_o()
{
/* This function reads the superblock of the partition, gets the root inode
 * and sends back the details of them. Note, that the FS_PROC_NR process does not
 * know the index of the vmnt object which refers to it, whenever the pathname 
 * lookup leaves a partition an ELEAVEMOUNT error is transferred back 
 * so that the VFS knows that it has to find the vnode on which this FS_PROC_NR 
 * process' partition is mounted on.
 */
  struct super_block *xp;
  struct inode *root_ip;
  int r = 0;
  phys_bytes ph;

  fs_dev = fs_m_in.REQ_DEV;

  /* Map the driver endpoint for this major */
  driver_endpoints[(fs_dev >> MAJOR) & BYTE].driver_e =  fs_m_in.REQ_DRIVER_E;
  boottime = fs_m_in.REQ_BOOTTIME;
  vfs_slink_storage = fs_m_in.REQ_SLINK_STORAGE;

  /* Fill in the super block. */
  superblock.s_dev = fs_dev;		/* read_super() needs to know which dev */
  r = read_super(&superblock);

  /* Is it recognized as a Minix filesystem? */
  if (r != 0) {
	superblock.s_dev = NO_DEV;
	return(r);
  }

  set_blocksize(superblock.s_block_size);
  
  /* Get the root inode of the mounted file system. */
  root_ip = NIL_INODE;		/* if 'r' not 0, make sure this is defined */
  if (r == 0) {
	if ( (root_ip = get_inode(fs_dev, ROOT_INODE)) == NIL_INODE) 
		r = err_code;
  }
  
  if (root_ip != NIL_INODE && root_ip->i_mode == 0) {
        put_inode(root_ip);
  	r = -EINVAL;
  }

  if (r != 0) return r;
  superblock.s_rd_only = fs_m_in.REQ_READONLY;
  superblock.s_is_root = fs_m_in.REQ_ISROOT;
  
  /* Root inode properties */
  fs_m_out.RES_INODE_NR = root_ip->i_num;
  fs_m_out.RES_MODE = root_ip->i_mode;
  fs_m_out.RES_FILE_SIZE = root_ip->i_size;

  /* Partition properties */
  fs_m_out.RES_MAXSIZE = superblock.s_max_size;
  fs_m_out.RES_BLOCKSIZE = superblock.s_block_size;
  
  return r;
}


/*===========================================================================*
 *				fs_mountpoint_o				     *
 *===========================================================================*/
int fs_mountpoint_o()
{
/* This function looks up the mount point, it checks the condition whether
 * the partition can be mounted on the inode or not. If ok, it gets the
 * mountpoint inode's details and stores the mounted vmnt's index (in the
 * vmnt table) so that it can be transferred back when the pathname lookup
 * encounters a mountpoint.
 */
  register struct inode *rip;
  int r = 0;
  mode_t bits;
  
  /* Get inode */
  caller_uid = fs_m_in.REQ_UID;
  caller_gid = fs_m_in.REQ_GID;
  
  /* Temporarily open the file. */
  if ( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE) {
printf("MFS(%d) get_inode by fs_mountpoint() failed\n", SELF_E);
        return(-EINVAL);
  }

  /* It may not be busy. */
  if (rip->i_count > 2)
  {
	printf("mfs:fs_mountpoint: i_count = %d\n", rip->i_count);
	r = -EBUSY;
  }

  /* It may not be special. */
  bits = rip->i_mode & I_TYPE;
  if (bits == I_BLOCK_SPECIAL || bits == I_CHAR_SPECIAL) r = -ENOTDIR;
	
  if ((rip->i_mode & I_TYPE) != I_DIRECTORY) r = -ENOTDIR;
      
  if (r != 0) {
      put_inode(rip);
      return r;
  }
  
  rip->i_mountpoint = TRUE;

  fs_m_out.m_source = rip->i_dev;/* Filled with the FS_PROC_NR endp by the system */
  fs_m_out.RES_INODE_NR = rip->i_num;
  fs_m_out.RES_FILE_SIZE = rip->i_size;
  fs_m_out.RES_MODE = rip->i_mode;

  return r;
}


/*===========================================================================*
 *				fs_mountpoint_s				     *
 *===========================================================================*/
int fs_mountpoint_s()
{
/* This function looks up the mount point, it checks the condition whether
 * the partition can be mounted on the inode or not. 
 */
  register struct inode *rip;
  int r = 0;
  mode_t bits;
  
  /* Temporarily open the file. */
  if ( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE) {
printf("MFS(%d) get_inode by fs_mountpoint() failed\n", SELF_E);
        return(-EINVAL);
  }

  if (rip->i_mountpoint)
	r= -EBUSY;

  /* It may not be special. */
  bits = rip->i_mode & I_TYPE;
  if (bits == I_BLOCK_SPECIAL || bits == I_CHAR_SPECIAL) r = -ENOTDIR;
	
  put_inode(rip);

  if (r == 0)
	rip->i_mountpoint = TRUE;
  return r;
}


/*===========================================================================*
 *				fs_unmount				     *
 *===========================================================================*/
int fs_unmount()
{
/* Unmount a file system by device number. */
  struct super_block *sp1;
  int count;
  register struct inode *rip;

  /* Close the device the file system lives on. */
  dev_close(driver_endpoints[(fs_dev >> MAJOR) & BYTE].driver_e, fs_dev);

  if(superblock.s_dev != fs_dev)
	return -EINVAL;
  
  /* See if the mounted device is busy.  Only 1 inode using it should be
   * open -- the root inode -- and that inode only 1 time.
   */
  count = 0;
  for (rip = &inode[0]; rip < &inode[NR_INODES]; rip++) {
	if (rip->i_count > 0 && rip->i_dev == fs_dev) {
		count += rip->i_count;
	}
  }
  
  if (count > 1) {
      return(-EBUSY);	/* can't umount a busy file system */
  }

  /* Sync the disk, and invalidate cache. */
  (void) fs_sync();		/* force any cached blocks out of memory */

  /* Finish off the unmount. */
  superblock.s_dev = NO_DEV;
  
  unmountdone = TRUE;

  return 0;
}



