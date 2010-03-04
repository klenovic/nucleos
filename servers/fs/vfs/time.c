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
 */

#include "fs.h"
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/utime.h>
#include "file.h"
#include <servers/vfs/fproc.h>
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
  time_t actime, modtime;
  struct vnode *vp;
  
  /* Adjust for case of 'timep' being NULL;
   * utime_strlen then holds the actual size: strlen(name)+1 */
  len = m_in.utime_length;
  if (len == 0) len = m_in.utime_strlen;

  /* Temporarily open the file */
  if (fetch_name(m_in.utime_file, len) != 0) return(err_code);
  if ((vp = eat_path(PATH_NOFLAGS)) == NIL_VNODE) return(err_code);
  
  /* Only the owner of a file or the super user can change its name. */  
  r = 0;
  if (vp->v_uid != fp->fp_effuid && fp->fp_effuid != SU_UID) r = -EPERM;
  if (m_in.utime_length == 0 && r != 0) r = forbidden(vp, W_BIT);
  if (read_only(vp) != 0) r = -EROFS; /* Not even su can touch if R/O */ 
  if (r == 0) {
	/* Issue request */
  if (m_in.utime_length == 0) {
        actime = modtime = clock_time();
  } else {
        actime = m_in.utime_actime;
        modtime = m_in.utime_modtime;
  }
	r = req_utime(vp->v_fs_e, vp->v_inode_nr, actime, modtime);
  }
  
  put_vnode(vp);
  return(r);
}

#define utime_ptimes	m2_l1	/* times pointer */

/*===========================================================================*
 *				sys_utime				     *
 *===========================================================================*/
int sys_utime(void)
{
	/* Perform the utime(name, timep) system call. */
	register int len;
	int r;
	time_t actime, modtime;
	struct utimbuf ut;
	struct vnode *vp;

	/* Adjust for case of 'timep' being NULL;
	 * utime_strlen then holds the actual size: strlen(name)+1 */
	len = m_in.utime_strlen;

	if (fetch_name(m_in.utime_file, len) != 0)
		return(err_code);

	if ((vp = eat_path(PATH_NOFLAGS)) == NIL_VNODE) return(err_code);

	/* get times buffer if not 0 */
	if (m_in.utime_ptimes != 0) {
		r = sys_datacopy(who_e, (vir_bytes)m_in.utime_ptimes, SELF, (vir_bytes)&ut, sizeof(struct utimbuf));

		if (r != 0)
			return r;
	}

	/* Only the owner of a file or the super user can change its name. */
	r = 0;
	if (vp->v_uid != fp->fp_effuid && fp->fp_effuid != SU_UID) r = -EPERM;
	if (m_in.utime_ptimes == 0 && r != 0) r = forbidden(vp, W_BIT);
	if (read_only(vp) != 0) r = -EROFS; /* Not even su can touch if R/O */

	if (r == 0) {
		/* Issue request */
		if (m_in.utime_ptimes == 0) {
			actime = modtime = clock_time();
		} else {
			actime = ut.actime;
			modtime = ut.modtime;
		}
		r = req_utime(vp->v_fs_e, vp->v_inode_nr, actime, modtime);
	}

	put_vnode(vp);

	return(r);
}
