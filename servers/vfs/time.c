/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file takes care of those system calls that deal with time.
 *
 * The entry points into this file are
 *   do_utime:		perform the UTIME system call
 *   do_stime:		PM informs FS_PROC_NR about STIME system call
 */

#include "fs.h"
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include "file.h"
#include "fproc.h"
#include "param.h"
#include "vnode.h"

#include <nucleos/vfsif.h>
#include "vmnt.h"

/*===========================================================================*
 *				do_utime				     *
 *===========================================================================*/
int do_utime()
{
/* Perform the utime(name, timep) system call. */
  register int len;
  int r;
  uid_t uid;
  time_t actime, modtime;
  struct vnode *vp;
  
  /* Adjust for case of 'timep' being NULL;
   * utime_strlen then holds the actual size: strlen(name)+1.
   */
  len = m_in.utime_length;
  if (len == 0) len = m_in.utime_strlen;

  if (fetch_name(m_in.utime_file, len) != 0) return(err_code);
  
  /* Request lookup */
  if ((r = lookup_vp(0 /*flags*/, 0 /*!use_realuid*/, &vp)) != 0) return r;

  /* Fill in request fields.*/
  if (m_in.utime_length == 0) {
        actime = modtime = clock_time();
  } else {
        actime = m_in.utime_actime;
        modtime = m_in.utime_modtime;
  }

  uid= fp->fp_effuid;

  r= 0;
  if (vp->v_uid != uid && uid != SU_UID) r = -EPERM;
  if (m_in.utime_length == 0 && r != 0)
  {
	/* With a null times pointer, updating the times (to the current time)
	 * is allow if the object is writable. 
	 */
	r = forbidden(vp, W_BIT, 0 /*!use_realuid*/);
  }

  if (r == 0)
  	r = read_only(vp);

  if (r != 0)
  {
	put_vnode(vp);
	return r;
  }
  
  /* Issue request */
  r= req_utime(vp->v_fs_e, vp->v_inode_nr, actime, modtime);
  put_vnode(vp);
  return r;
}

