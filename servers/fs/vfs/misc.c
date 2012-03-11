/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains a collection of miscellaneous procedures.  Some of them
 * perform simple system calls.  Some others do a little part of system calls
 * that are mostly performed by the Memory Manager.
 *
 * The entry points into this file are
 *   do_dup:	  perform the DUP system call
 *   do_fcntl:	  perform the FCNTL system call
 *   do_sync:	  perform the SYNC system call
 *   do_fsync:	  perform the FSYNC system call
 *   do_reboot:	  sync disks and prepare for shutdown
 *   do_fork:	  adjust the tables after PM_PROC_NR has performed a FORK system call
 *   do_exec:	  handle files with FD_CLOEXEC on after PM_PROC_NR has done an EXEC
 *   do_exit:	  a process has exited; note that in the tables
 *   do_set:	  set uid or gid for some process
 *   do_revive:	  revive a process that was waiting for something (e.g. TTY)
 *   do_svrctl:	  file system control
 *   do_getsysinfo:	request copy of VFS_PROC_NR data structure
 *   pm_dumpcore: create a core dump
 */

#include "fs.h"
#include <nucleos/fcntl.h>
#include <assert.h>
#include <nucleos/unistd.h>
#include <nucleos/safecopies.h>
#include <nucleos/endpoint.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include <nucleos/time.h>
#include <nucleos/types.h>
#include <nucleos/ptrace.h>
#include <nucleos/svrctl.h>
#include <asm/ioctls.h>
#include "file.h"
#include <servers/fs/vfs/fproc.h>
#include "param.h"

#include <nucleos/vfsif.h>
#include "vnode.h"
#include "vmnt.h"

#define CORE_NAME	"core"
#define CORE_MODE	0777	/* mode to use on core image files */

#ifdef CONFIG_DEBUG_SERVERS_SYSCALL_STATS
unsigned long calls_stats[NR_syscalls];
#endif

static void free_proc(struct fproc *freed, int flags);
static void unmount_all(void);

#define FP_EXITING	1

/*===========================================================================*
 *				do_getsysinfo				     *
 *===========================================================================*/
int do_getsysinfo()
{
  struct fproc *proc_addr;
  vir_bytes src_addr, dst_addr;
  size_t len;
  int s;

  /* Only su may call do_getsysinfo. This call may leak information (and is not
   * stable enough to be part of the API/ABI). 
			 */

  if (!super_user) return(-EPERM);

  switch(m_in.info_what) {
  case SI_PROC_ADDR:
  	proc_addr = &fproc[0];
  	src_addr = (vir_bytes) &proc_addr;
  	len = sizeof(struct fproc *);
  	break; 
  case SI_PROC_TAB:
  	src_addr = (vir_bytes) fproc;
  	len = sizeof(struct fproc) * NR_PROCS;
  	break; 
  case SI_DMAP_TAB:
  	src_addr = (vir_bytes) dmap;
  	len = sizeof(struct dmap) * NR_DEVICES;
  	break; 
#ifdef CONFIG_DEBUG_SERVERS_SYSCALL_STATS
  case SI_CALL_STATS:
  	src_addr = (vir_bytes) calls_stats;
  	len = sizeof(calls_stats);
  	break; 
#endif
  default:
  	return(-EINVAL);
  }

  dst_addr = (vir_bytes) m_in.info_where;
  if (0 != (s = sys_datacopy(ENDPT_SELF, src_addr, who_e, dst_addr, len))) return(s);
  return(0);

}

/*===========================================================================*
 *				do_dup					     *
 *===========================================================================*/
int do_dup()
{
/* Perform the dup(fd) or dup2(fd,fd2) system call. */
  register int rfd;
  register struct filp *f;
  struct filp *dummy;
  int r;

  /* Is the file descriptor valid? */
  rfd = m_in.fd & ~DUP_MASK;		/* kill off dup2 bit, if on */
  if ((f = get_filp(rfd)) == NIL_FILP) return(err_code);

  /* Distinguish between dup and dup2. */
  if (m_in.fd == rfd) {			/* bit not on */
	/* dup(fd) */
	if ( (r = get_fd(0, 0, &m_in.fd2, &dummy)) != 0) return(r);
  } else {
	/* dup2(fd, fd2) */
	if (m_in.fd2 < 0 || m_in.fd2 >= OPEN_MAX) return(-EBADF);
	if (rfd == m_in.fd2) return(m_in.fd2);	/* ignore the call: dup2(x, x) */
	m_in.fd = m_in.fd2;		/* prepare to close fd2 */
	(void) do_close();	/* cannot fail */
  }

  /* Success. Set up new file descriptors. */
  f->filp_count++;
  fp->fp_filp[m_in.fd2] = f;
  FD_SET(m_in.fd2, &fp->fp_filp_inuse);
  return(m_in.fd2);
}

/*===========================================================================*
 *				do_dup2					     *
 *===========================================================================*/
int do_dup2(void)
{
	/* Perform the dup2(oldfd, newfd) system call. */
	register int oldfd;
	register struct filp *f;

	oldfd = m_in.fd;

	/* Check whether oldfd is a valid file descriptor */
	if ((f = get_filp(oldfd)) == NIL_FILP)
		return -EBADF;

	if (m_in.fd2 < 0 || m_in.fd2 >= OPEN_MAX)
		return -EBADF;

	if (oldfd == m_in.fd2)
		return(m_in.fd2);	/* ignore the call: dup2(x, x) */

	m_in.fd = m_in.fd2;		/* prepare to close fd2 */

	/* close newfd */
	(void) do_close();		/* cannot fail */

	/* Success. Set up new file descriptors. */
	f->filp_count++;

	fp->fp_filp[m_in.fd2] = f;

	FD_SET(m_in.fd2, &fp->fp_filp_inuse);

	return(m_in.fd2);
}

/*===========================================================================*
 *				do_fcntl				     *
 *===========================================================================*/
int do_fcntl()
{
/* Perform the fcntl(fd, request, ...) system call. */

  register struct filp *f;
  int new_fd, r, fl;
  long cloexec_mask;		/* bit map for the FD_CLOEXEC flag */
  long clo_value;		/* FD_CLOEXEC flag in proper position */
  struct filp *dummy;

  /* Is the file descriptor valid? */
  if ((f = get_filp(m_in.fd)) == NIL_FILP) return(err_code);
  
  switch (m_in.request) {
     case F_DUPFD:
	/* This replaces the old dup() system call. */
	if (m_in.addr < 0 || m_in.addr >= OPEN_MAX) return(-EINVAL);
	if ((r = get_fd(m_in.addr, 0, &new_fd, &dummy)) != 0) return(r);
	f->filp_count++;
	fp->fp_filp[new_fd] = f;
	return(new_fd);

     case F_GETFD:
	/* Get close-on-exec flag (FD_CLOEXEC in POSIX Table 6-2). */
	return( FD_ISSET(m_in.fd, &fp->fp_cloexec_set) ? FD_CLOEXEC : 0);

     case F_SETFD:
	/* Set close-on-exec flag (FD_CLOEXEC in POSIX Table 6-2). */
	if(m_in.addr & FD_CLOEXEC)
		FD_SET(m_in.fd, &fp->fp_cloexec_set);
	else
		FD_CLR(m_in.fd, &fp->fp_cloexec_set);
	return 0;

     case F_GETFL:
	/* Get file status flags (O_NONBLOCK and O_APPEND). */
	fl = f->filp_flags & (O_NONBLOCK | O_APPEND | O_ACCMODE);
	return(fl);	

     case F_SETFL:
	/* Set file status flags (O_NONBLOCK and O_APPEND). */
	fl = O_NONBLOCK | O_APPEND | O_REOPEN;
	f->filp_flags = (f->filp_flags & ~fl) | (m_in.addr & fl);
	return 0;

     case F_GETLK:
     case F_SETLK:
     case F_SETLKW:
	/* Set or clear a file lock. */
	r = lock_op(f, m_in.request);
	return(r);

     case F_FREESP:
     {
	/* Free a section of a file. Preparation is done here, actual freeing
	 * in freesp_inode().
	 */
	off_t start, end;
	struct flock flock_arg;
	signed long offset;

	/* Check if it's a regular file. */
	if((f->filp_vno->v_mode & I_TYPE) != I_REGULAR) return(-EINVAL);
	if (!(f->filp_mode & W_BIT)) return(-EBADF);

	/* Copy flock data from userspace. */
	if((r = sys_datacopy(who_e, (vir_bytes) m_in.name1, ENDPT_SELF, 
	     (vir_bytes) &flock_arg, (phys_bytes) sizeof(flock_arg))) != 0)
		return(r);

	/* Convert starting offset to signed. */
	offset = (signed long) flock_arg.l_start;

	/* Figure out starting position base. */
	switch(flock_arg.l_whence) {
		case SEEK_SET: start = 0; if(offset < 0) return -EINVAL; break;
		case SEEK_CUR:
			if (ex64hi(f->filp_pos) != 0)
			panic(__FILE__, "do_fcntl: position in file too high",
					NO_NUM);
		
	      start = ex64lo(f->filp_pos);
	      break;
		case SEEK_END: start = f->filp_vno->v_size; break;
		default: return -EINVAL;
	}

	/* Check for overflow or underflow. */
	if(offset > 0 && start + offset < start) return -EINVAL;
	if(offset < 0 && start + offset > start) return -EINVAL; 
	start += offset;
	if(flock_arg.l_len > 0) {
		end = start + flock_arg.l_len;
		if(end <= start) return -EINVAL;
	} else {
                end = 0;
	}
  
        return req_ftrunc(f->filp_vno->v_fs_e, f->filp_vno->v_inode_nr, start,
        		  end);
     }

     default:
	return(-EINVAL);
  }
}


/*===========================================================================*
 *				do_sync					     *
 *===========================================================================*/
int do_sync()
{
  struct vmnt *vmp;
  for (vmp = &vmnt[1]; vmp < &vmnt[NR_MNTS]; ++vmp) 
	  if (vmp->m_dev != NO_DEV) 
                  req_sync(vmp->m_fs_e);

  return(0);
}

/*===========================================================================*
 *				do_fsync				     *
 *===========================================================================*/
int do_fsync()
{
/* Perform the fsync() system call. For now, don't be unnecessarily smart. */

  do_sync();

  return 0;
}

/*===========================================================================*
 *				unmount_all				     *
 *===========================================================================*/
static void unmount_all(void)
{
  /* Unmount all filesystems.  File systems are mounted on other file systems,
   * so you have to pull off the loose bits repeatedly to get it all undone.
   */

  int i;
  for (i= 0; i < NR_MNTS; i++) {
  	struct vmnt *vmp;

	/* Unmount at least one. */
	for (vmp = &vmnt[0]; vmp < &vmnt[NR_MNTS]; vmp++) {
		if (vmp->m_dev != NO_DEV) 
			unmount(vmp->m_dev); 
	}
  }
}

/*===========================================================================*
 *				pm_reboot				     *
 *===========================================================================*/
void pm_reboot()
{
  /* Perform the VFS_PROC_NR side of the reboot call. */
  int i;

  do_sync();

  SANITYCHECK;

  /* Do exit processing for all leftover processes and servers,
   * but don't actually exit them (if they were really gone, PM
   * will tell us about it).
   */
  for (i = 0; i < NR_PROCS; i++)
	if((m_in.endpt1 = fproc[i].fp_endpoint) != ENDPT_NONE) {
		/* No FP_EXITING, just free the resources, otherwise
		 * consistency check for fp_endpoint (set to ENDPT_NONE) will
		 * fail if process wants to do something in the (short)
		 * future.
		 */
		free_proc(&fproc[i], 0);
	}
  SANITYCHECK;

  unmount_all();

  SANITYCHECK;

}

/*===========================================================================*
 *				pm_fork					     *
 *===========================================================================*/
void pm_fork(pproc, cproc, cpid)
int pproc;	/* Parent process */
int cproc;	/* Child process */
int cpid;	/* Child process id */
{
/* Perform those aspects of the fork() system call that relate to files.
 * In particular, let the child inherit its parent's file descriptors.
 * The parent and child parameters tell who forked off whom. The file
 * system uses the same slot numbers as the kernel.  Only PM_PROC_NR makes this call.
 */

  register struct fproc *cp;
  int i, parentno, childno;

  /* Check up-to-dateness of fproc. */
  okendpt(pproc, &parentno);

  /* PM gives child endpoint, which implies process slot information.
   * Don't call isokendpt, because that will verify if the endpoint
   * number is correct in fproc, which it won't be.
   */
  childno = _ENDPOINT_P(cproc);
  if(childno < 0 || childno >= NR_PROCS)
	panic(__FILE__, "VFS_PROC_NR: bogus child for forking", m_in.child_endpt);
  if(fproc[childno].fp_pid != PID_FREE)
	panic(__FILE__, "VFS_PROC_NR: forking on top of in-use child", childno);

  /* Copy the parent's fproc struct to the child. */
  fproc[childno] = fproc[parentno];

  /* Increase the counters in the 'filp' table. */
  cp = &fproc[childno];
  fp = &fproc[parentno];

  for (i = 0; i < OPEN_MAX; i++)
	if (cp->fp_filp[i] != NIL_FILP) cp->fp_filp[i]->filp_count++;

  /* Fill in new process and endpoint id. */
  cp->fp_pid = cpid;
  cp->fp_endpoint = cproc;

  /* A forking process never has an outstanding grant,
   * as it isn't blocking on i/o.
   */
  if(GRANT_VALID(fp->fp_grant)) {
	printk("vfs: fork: fp (endpoint %d) has grant %d\n", fp->fp_endpoint, fp->fp_grant);
	panic(__FILE__, "fp contains valid grant", NO_NUM);
  }
  if(GRANT_VALID(cp->fp_grant)) {
	printk("vfs: fork: cp (endpoint %d) has grant %d\n", cp->fp_endpoint, cp->fp_grant);
	panic(__FILE__, "cp contains valid grant", NO_NUM);
  }

  /* A child is not a process leader. */
  cp->fp_sesldr = 0;

  /* This child has not exec()ced yet. */
  cp->fp_execced = 0;

  /* Record the fact that both root and working dir have another user. */
  if(cp->fp_rd) dup_vnode(cp->fp_rd);
  if(cp->fp_wd) dup_vnode(cp->fp_wd);
}

/*===========================================================================*
 *				free_proc				     *
 *===========================================================================*/
static void free_proc(struct fproc *exiter, int flags)
{
  int i;
  register struct fproc *rfp;
  register struct filp *rfilp;
  register struct vnode *vp;
  dev_t dev;

 SANITYCHECK;

  fp = exiter;		/* get_filp() needs 'fp' */

  if(fp->fp_endpoint == ENDPT_NONE) {
	panic(__FILE__, "free_proc: already free", NO_NUM);
  }

  if (fp_is_blocked(fp)) {
 	SANITYCHECK;
	unpause(fp->fp_endpoint);
 	SANITYCHECK;
  }

 SANITYCHECK;

  /* Loop on file descriptors, closing any that are open. */
  for (i = 0; i < OPEN_MAX; i++) {
	(void) close_fd(fp, i);
  }
  
  /* Check if any process is SUSPENDed on this driver.
   * If a driver exits, unmap its entries in the dmap table.
   * (unmapping has to be done after the first step, because the
   * dmap table is used in the first step.)
   */
  unsuspend_by_endpt(fp->fp_endpoint);

  /* Release root and working directories. */
  if(fp->fp_rd) { put_vnode(fp->fp_rd); fp->fp_rd = NIL_VNODE; }
  if(fp->fp_wd) { put_vnode(fp->fp_wd); fp->fp_wd = NIL_VNODE; }

  /* The rest of these actions is only done when processes actually
   * exit.
   */
  if(!(flags & FP_EXITING)) {
 	SANITYCHECK;
	return;
  }

  /* Invalidate endpoint number for error and sanity checks. */
  fp->fp_endpoint = ENDPT_NONE;

  /* If a session leader exits and it has a controlling tty, then revoke 
   * access to its controlling tty from all other processes using it.
   */
  if (fp->fp_sesldr && fp->fp_tty != 0) {

      dev = fp->fp_tty;

      for (rfp = &fproc[0]; rfp < &fproc[NR_PROCS]; rfp++) {
	  if(rfp->fp_pid == PID_FREE) continue;
          if (rfp->fp_tty == dev) rfp->fp_tty = 0;

          for (i = 0; i < OPEN_MAX; i++) {
		if ((rfilp = rfp->fp_filp[i]) == NIL_FILP) continue;
		if (rfilp->filp_mode == FILP_CLOSED) continue;
		vp = rfilp->filp_vno;
		if ((vp->v_mode & I_TYPE) != I_CHAR_SPECIAL) continue;
		if ((dev_t) vp->v_sdev != dev) continue;

		(void) dev_close(dev, rfilp-filp);
		/* Ignore any errors, even SUSPEND. */

		rfilp->filp_mode = FILP_CLOSED;
          }
      }
  }

  /* Exit done. Mark slot as free. */
  fp->fp_pid = PID_FREE;

  SANITYCHECK;
}

/*===========================================================================*
 *				pm_exit					     *
 *===========================================================================*/
void pm_exit(proc)
int proc;
{
  int exitee_p;
/* Perform the file system portion of the exit(status) system call. */

  /* Nevertheless, pretend that the call came from the user. */
  okendpt(proc, &exitee_p);
  free_proc(&fproc[exitee_p], FP_EXITING);
}

/*===========================================================================*
 *				pm_setgid				     *
 *===========================================================================*/
void pm_setgid(proc_e, egid, rgid)
int proc_e;
int egid;
int rgid;
{
  register struct fproc *tfp;
  int slot;

  okendpt(proc_e, &slot);
  tfp = &fproc[slot];

  tfp->fp_effgid =  egid;
  tfp->fp_realgid = rgid;
}


/*===========================================================================*
 *				pm_setgroups				     *
 *===========================================================================*/
void pm_setgroups(proc_e, ngroups, groups)
int proc_e;
int ngroups;
gid_t *groups;
{
  struct fproc *rfp;
  int slot, i;

  okendpt(proc_e, &slot);
  rfp = &fproc[slot];
  if (ngroups * sizeof(gid_t) > sizeof(rfp->fp_sgroups))
  	panic(__FILE__, "VFS: pm_setgroups: too much data to copy\n", NO_NUM);
  if(sys_datacopy(who_e, (vir_bytes) groups, ENDPT_SELF, (vir_bytes) rfp->fp_sgroups,
  		   ngroups * sizeof(gid_t)) == 0) {
	rfp->fp_ngroups = ngroups;
  } else
  	panic(__FILE__, "VFS: pm_setgroups: datacopy failed\n", NO_NUM);
}


/*===========================================================================*
 *				pm_setuid				     *
 *===========================================================================*/
void pm_setuid(proc_e, euid, ruid)
int proc_e;
int euid;
int ruid;
{
  register struct fproc *tfp;
  int slot;

  okendpt(proc_e, &slot);
  tfp = &fproc[slot];

  tfp->fp_effuid =  euid;
  tfp->fp_realuid = ruid;
}

/*===========================================================================*
 *				do_svrctl				     *
 *===========================================================================*/
int do_svrctl()
{
  switch (m_in.svrctl_req) {
  case FSSIGNON: {
	/* A server in user space calls in to manage a device. */
	struct fssignon device;
	int r, major, proc_nr_n;

	if (fp->fp_effuid != SU_UID && fp->fp_effuid != SERVERS_UID)
		return(-EPERM);

	/* Try to copy request structure to VFS_PROC_NR. */
	if ((r = sys_datacopy(who_e, (vir_bytes) m_in.svrctl_argp,
		VFS_PROC_NR, (vir_bytes) &device,
		(phys_bytes) sizeof(device))) != 0) 
	    return(r);

	if (isokendpt(who_e, &proc_nr_n) != 0)
		return(-EINVAL);

	/* Try to update device mapping. */
	major = (device.dev >> MAJOR) & BYTE;
	r=map_driver(major, who_e, device.style, 0 /* !force */);
	if (r == 0)
	{
		/* If a driver has completed its exec(), it can be announced
		 * to be up.
		*/
		if(fproc[proc_nr_n].fp_execced) {
			/* Reply before calling dev_up */
#if 0
			printk("do_svrctl: replying before dev_up\n");
#endif
			reply(who_e, r);
			dev_up(major);
			r= SUSPEND;
		} else {
			dmap[major].dmap_flags |= DMAP_BABY;
		}
	}

	return(r);
  }
  case FSDEVUNMAP: {
	struct fsdevunmap fdu;
	int r, major;
	/* Try to copy request structure to VFS_PROC_NR. */
	if ((r = sys_datacopy(who_e, (vir_bytes) m_in.svrctl_argp,
		VFS_PROC_NR, (vir_bytes) &fdu,
		(phys_bytes) sizeof(fdu))) != 0) 
	    return(r);
	major = (fdu.dev >> MAJOR) & BYTE;
	r=map_driver(major, ENDPT_NONE, 0, 0);
	return(r);
  }
  default:
	return(-EINVAL);
  }
}

/*===========================================================================*
 *				pm_dumpcore				     *
 *===========================================================================*/
int pm_dumpcore(proc_e, seg_ptr)
int proc_e;
struct mem_map *seg_ptr;
{
	int r, proc_s;
	
	/* Terminate the process */
	okendpt(proc_e, &proc_s);
	free_proc(&fproc[proc_s], FP_EXITING);
        
	return 0;
}
