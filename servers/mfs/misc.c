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
#include <nucleos/vfsif.h>

#include "buf.h"
#include "inode.h"


/*===========================================================================*
 *				fs_sync					     *
 *===========================================================================*/
int fs_sync()
{
/* Perform the sync() system call.  Flush all the tables. 
 * The order in which the various tables are flushed is critical.  The
 * blocks must be flushed last, since rw_inode() leaves its results in
 * the block cache.
 */
  register struct inode *rip;
  register struct buf *bp;

  /* Write all the dirty inodes to the disk. */
  for (rip = &inode[0]; rip < &inode[NR_INODES]; rip++)
	if (rip->i_count > 0 && rip->i_dirt == DIRTY) rw_inode(rip, WRITING);

  /* Write all the dirty blocks to the disk, one drive at a time. */
  for (bp = &buf[0]; bp < &buf[NR_BUFS]; bp++)
	if (bp->b_dev != NO_DEV && bp->b_dirt == DIRTY) 
            flushall(bp->b_dev);

  return 0;		/* sync() can't fail */
}


/*===========================================================================*
 *				fs_flush				     *
 *===========================================================================*/
int fs_flush()
{
/* Flush the blocks of a device from the cache after writing any dirty blocks
 * to disk.
 */
  dev_t dev;

  dev= fs_m_in.REQ_DEV;
  if (dev == fs_dev)
  {
	return -EBUSY;
  }
  flushall(dev);
  invalidate(dev);

  return 0;
}


