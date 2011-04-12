#include "fs.h"
#include "inode.h"
#include <nucleos/stat.h>

static int stat_inode(struct pipe_inode *rip, int who_e, cp_grant_id_t gid);

/**
 * Common code for stat and fstat system calls.
 * @param rip   pointer to inode to stat
 * @param who_e  caller endpoint
 * @param gid  grant for the stat buf
 * @return 0 on success
 */
static int stat_inode(struct pipe_inode *rip, int who_e, cp_grant_id_t gid)
{
	struct kstat ksb;
	int r, s;

	/* Update the atime, ctime, and mtime fields in the inode, if need be. */
	if (rip->i_update) update_times(rip);

	ksb.dev = rip->i_dev;
	ksb.ino = rip->i_num;
	ksb.mode = rip->i_mode;
	ksb.nlink = rip->i_nlinks;
	ksb.uid = rip->i_uid;
	ksb.gid = rip->i_gid;
	ksb.rdev = (dev_t) 0;
	ksb.size = rip->i_size;
	ksb.mode &= ~I_REGULAR;	/* wipe out I_REGULAR bit for pipes */

	ksb.blksize = 1024;
	/* @nucleos: what it should be for pipes? */
	ksb.blocks = 0;

	ksb.atime.tv_sec = rip->i_atime;
	ksb.mtime.tv_sec = rip->i_mtime;
	ksb.ctime.tv_sec = rip->i_ctime;
	ksb.atime.tv_nsec = 0;
	ksb.mtime.tv_nsec = 0;
	ksb.ctime.tv_nsec = 0;

	/* Copy the struct to caller space. */
	r = sys_safecopyto(who_e, gid, 0, (vir_bytes)&ksb,(phys_bytes)sizeof(ksb), D);

	return(r);
}

int fs_stat(void)
{
	register int r;			/* return value */
	register struct pipe_inode *rip;	/* target inode */

	if((rip = find_inode(fs_m_in.REQ_INODE_NR)) == NIL_INODE)
		return(-EINVAL);

	get_inode(rip->i_dev, rip->i_num);	/* mark inode in use */

	r = stat_inode(rip, fs_m_in.m_source, fs_m_in.REQ_GRANT);

	put_inode(rip);			/* release the inode */

	return(r);
}
