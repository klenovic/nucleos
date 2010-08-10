/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the code for performing four system calls relating to
 * status and directories.
 *
 * The entry points into this file are
 *   do_chdir:	perform the CHDIR system call
 *   do_chroot:	perform the CHROOT system call
 *   do_lstat:  perform the LSTAT system call
 *   do_stat:	perform the STAT system call
 *   do_fstat:	perform the FSTAT system call
 *   do_fstatfs: perform the FSTATFS system call
 */

#include "fs.h"
#include <nucleos/stat.h>
#include <nucleos/statfs.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include <nucleos/string.h>
#include "file.h"
#include <servers/vfs/fproc.h>
#include "param.h"

#include <nucleos/vfsif.h>
#include "vnode.h"
#include "vmnt.h"

static int change(struct vnode **iip, char *name_ptr);
static int change_into(struct vnode **iip, struct vnode *vp);

/**
 * Convert kstat to stat structure
 */
static inline void vfs_kstat_to_stat(struct stat *sb, struct kstat *ksb)
{
	sb->st_dev = ksb->dev;
	sb->st_ino = ksb->ino;
	sb->st_mode = ksb->mode;
	sb->st_nlink = ksb->nlink;
	sb->st_uid = ksb->uid;
	sb->st_gid = ksb->gid;
	sb->st_rdev = ksb->rdev;
	sb->st_size = ksb->size;
	sb->st_blksize = ksb->blksize;
	sb->st_blocks = ksb->blocks;
	sb->st_atime = ksb->atime.tv_sec;
	sb->st_mtime = ksb->mtime.tv_sec;
	sb->st_ctime = ksb->ctime.tv_sec;
#ifdef STAT_HAVE_NSEC
	sb->st_atime_nsec = ksb->atime.tv_nsec;
	sb->st_mtime_nsec = ksb->mtime.tv_nsec;
	sb->st_ctime_nsec = ksb->ctime.tv_nsec;
#endif
}

/**
 * Change directory on already-opened fd.
 */
int do_fchdir(void)
{

	struct filp *rfilp;

	/* Is the file descriptor valid? */
	if ((rfilp = get_filp(m_in.fd)) == NIL_FILP)
		return(err_code);

	dup_vnode(rfilp->filp_vno);	/* Change into expects a reference. */

	return change_into(&fp->fp_wd, rfilp->filp_vno);
}

/**
 * Perform the chdir(name) system call.
 */
int do_chdir(void)
{
	/* Change directory.  This function is  also called by PM_PROC_NR to simulate a chdir
	 * in order to do EXEC, etc.  It also changes the root directory, the uids and
	 * gids, and the umask. 
	 */
	int r;
	register struct fproc *rfp;

	if (who_e == PM_PROC_NR) {
		int slot;
		if(isokendpt(m_in.endpt1, &slot) != 0)
			return(-EINVAL);

		rfp = &fproc[slot];

		put_vnode(fp->fp_rd);
		dup_vnode(fp->fp_rd = rfp->fp_rd);
		put_vnode(fp->fp_wd);
		dup_vnode(fp->fp_wd = rfp->fp_wd);

		/* PM_PROC_NR uses access() to check permissions.  To make this work, pretend
		 * that the user's real ids are the same as the user's effective ids.
		 * FS_PROC_NR calls other than access() do not use the real ids, so are not
		 * affected.
		 */
		fp->fp_realuid = fp->fp_effuid = rfp->fp_effuid;
		fp->fp_realgid = fp->fp_effgid = rfp->fp_effgid;
		fp->fp_umask = rfp->fp_umask;

		return 0;
	}

	return change(&fp->fp_wd, m_in.name);
}

/**
 * Perform the chroot(name) system call.
 */
int do_chroot(void)
{
	if (!super_user)
		return(-EPERM);	/* only su may chroot() */

	return change(&fp->fp_rd, m_in.name);
}

/*===========================================================================*
 *				change					     *
 *===========================================================================*/
static int change(iip, name_ptr)
struct vnode **iip;		/* pointer to the inode pointer for the dir */
char *name_ptr;			/* pointer to the directory name to change to */
{
	/* Do the actual work for chdir() and chroot(). */
	struct vnode *vp;
	int r;

	/* Try to open the directory */
	if (fetch_name(user_fullpath, PATH_MAX, name_ptr) < 0)
		return(err_code);

	if ((vp = eat_path(PATH_NOFLAGS)) == NIL_VNODE)
		return(err_code);

	return change_into(iip, vp);
}

static int change_into(iip, vp)
struct vnode **iip;	/* pointer to the inode pointer for the dir */
struct vnode *vp;	/* this is what the inode has to become */
{
	int r;

	/* It must be a directory and also be searchable */
	if ((vp->v_mode & I_TYPE) != I_DIRECTORY)
		r = -ENOTDIR;
	else
		r = forbidden(vp, X_BIT);	/* Check if dir is searchable*/

	/* If error, return vnode */
	if (r != 0) {
		put_vnode(vp);
		return(r);
	}

	/* Everything is 0.  Make the change. */
	put_vnode(*iip);	/* release the old directory */
	*iip = vp;		/* acquire the new one */

	return 0;
}

/**
 * Perform the stat(name, buf) system call.
 */
int do_stat(void)
{
	int r;
	struct vnode *vp;
	struct stat sb;
	struct kstat ksb;

	if (fetch_name(user_fullpath, PATH_MAX, m_in.name1) < 0)
		return(err_code);

	if ((vp = eat_path(PATH_NOFLAGS)) == NIL_VNODE)
		return(err_code);

	r = req_stat(vp->v_fs_e, vp->v_inode_nr, FS_PROC_NR, &ksb, 0);

	if (!r) {
		memset(&sb, 0, sizeof(struct stat));

		vfs_kstat_to_stat(&sb, &ksb);

		if (!m_in.name2)
			return -EFAULT;

		/* copy to user */
		r = sys_vircopy(FS_PROC_NR, D, (vir_bytes)&sb, who_e, D, (vir_bytes)m_in.name2,
				sizeof(struct stat));

		if (r) {
			put_vnode(vp);
			return -EFAULT;
		}
	}

	put_vnode(vp);

	return r;
}

/**
 * Perform the fstat(fd, buf) system call.
 */
int do_fstat(void)
{
	register struct filp *rfilp;
	int pipe_pos = 0;
	struct stat sb;
	struct kstat ksb;
	int r;

	/* Is the file descriptor valid? */
	if ((rfilp = get_filp(m_in.fd)) == NIL_FILP)
		return(err_code);

	/* If we read from a pipe, send position too */
	if (rfilp->filp_vno->v_pipe == I_PIPE) {
		if (rfilp->filp_mode & R_BIT)
			if (ex64hi(rfilp->filp_pos) != 0) {
				panic(__FILE__, "do_fstat: bad position in pipe", NO_NUM);
			}
		pipe_pos = ex64lo(rfilp->filp_pos);
	}

	r = req_stat(rfilp->filp_vno->v_fs_e, rfilp->filp_vno->v_inode_nr,
		     FS_PROC_NR, &ksb, pipe_pos);

	if (!r) {
		memset(&sb, 0, sizeof(struct stat));

		if (pipe_pos)
			ksb.size -= pipe_pos;

		vfs_kstat_to_stat(&sb, &ksb);

		if (!m_in.buffer)
			return -EFAULT;

		r = sys_vircopy(FS_PROC_NR, D, (vir_bytes)&sb, who_e, D, (vir_bytes)m_in.buffer,
				sizeof(struct stat));

		if (r)
			return -EFAULT;
	}

	return 0;
}

/**
 * Perform the fstatfs(fd, buf) system call.
 */
int do_fstatfs(void)
{
	struct filp *rfilp;

	/* Is the file descriptor valid? */
	if ((rfilp = get_filp(m_in.fd)) == NIL_FILP)
		return(err_code);

	return req_fstatfs(rfilp->filp_vno->v_fs_e, who_e, m_in.buffer);
}

/**
 * Perform the lstat(name, buf) system call.
 */
int do_lstat(void)
{
	struct vnode *vp;
	struct kstat ksb;
	struct stat sb;
	int r;

	if (fetch_name(user_fullpath, PATH_MAX, m_in.name1) < 0)
		return(err_code);

	if ((vp = eat_path(PATH_RET_SYMLINK)) == NIL_VNODE)
		return(err_code);

	r = req_stat(vp->v_fs_e, vp->v_inode_nr, FS_PROC_NR, &ksb, 0);

	if (!r) {
		memset(&sb, 0, sizeof(struct stat));

		vfs_kstat_to_stat(&sb, &ksb);

		if (!m_in.name2)
			return -EFAULT;

		r = sys_vircopy(FS_PROC_NR, D, (vir_bytes)&sb, who_e, D, (vir_bytes)m_in.name2,
				sizeof(struct stat));

		if (r) {
			put_vnode(vp);
			return -EFAULT;
		}
	}

	put_vnode(vp);

	return(r);
}
