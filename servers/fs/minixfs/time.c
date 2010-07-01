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
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <servers/mfs/inode.h>

#include <nucleos/vfsif.h>


/*===========================================================================*
 *				fs_utime				     *
 *===========================================================================*/
int fs_utime()
{
  register struct inode *rip;
  register int r;
  
  /* Temporarily open the file. */
  if( (rip = get_inode(fs_dev, fs_m_in.REQ_INODE_NR)) == NIL_INODE)
        return(-EINVAL);

  /* Only the owner of a file or the super_user can change its time. */
  r = 0;
  if (read_only(rip) != 0) r = -EROFS;	/* not even su can touch if R/O */
  if (r == 0) {
	rip->i_atime = fs_m_in.REQ_ACTIME;
	rip->i_mtime = fs_m_in.REQ_MODTIME;

	rip->i_update = CTIME;	/* discard any stale ATIME and MTIME flags */
	rip->i_dirt = DIRTY;
  }

  put_inode(rip);
  return(r);
}

