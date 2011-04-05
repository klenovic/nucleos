/* Created (MFS based):
 *   February 2010 (Evgeniy Ivanov)
 */

#include "fs.h"
#include <nucleos/stat.h>
#include <nucleos/statfs.h>
#include "inode.h"
#include "super.h"
#include <nucleos/vfsif.h>


/*===========================================================================*
 *				stat_inode				     *
 *===========================================================================*/
static int stat_inode(
  register struct inode *rip,	/* pointer to inode to stat */
  endpoint_t who_e,		/* Caller endpoint */
  cp_grant_id_t gid		/* grant for the stat buf */
)
{
/* Common code for stat and fstat system calls. */

  struct kstat ksb;
  mode_t mo;
  int r, s;

  /* Update the atime, ctime, and mtime fields in the inode, if need be. */
  if (rip->i_update) update_times(rip);

  /* Fill in the statbuf struct. */
  mo = rip->i_mode & I_TYPE;

  /* true iff special */
  s = (mo == I_CHAR_SPECIAL || mo == I_BLOCK_SPECIAL);

  ksb.dev = rip->i_dev;
  ksb.ino = rip->i_num;
  ksb.mode = rip->i_mode;
  ksb.nlink = rip->i_links_count;
  ksb.uid = rip->i_uid;
  ksb.gid = rip->i_gid;
  ksb.rdev = (s ? rip->i_block[0] : NO_DEV);
  ksb.size = rip->i_size;
  ksb.atime.tv_sec = rip->i_atime;
  ksb.mtime.tv_sec = rip->i_mtime;
  ksb.ctime.tv_sec = rip->i_ctime;
  ksb.atime.tv_nsec = 0;
  ksb.mtime.tv_nsec = 0;
  ksb.ctime.tv_nsec = 0;

  /* @nucleos: FIXIT for ext2 */
  ksb.blksize = 1024;
  ksb.blocks = ((ksb.size + 511) >> 9);

  /* Copy the struct to user space. */
  r = sys_safecopyto(who_e, gid, (vir_bytes)0, (vir_bytes)&ksb, (size_t) sizeof(ksb), D);

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

  if((rip = find_inode(fs_dev, ROOT_INODE)) == NULL)
	  return(-EINVAL);

  st.f_bsize = rip->i_sp->s_block_size;

  /* Copy the struct to user space. */
  r = sys_safecopyto(fs_m_in.m_source, (cp_grant_id_t) fs_m_in.REQ_GRANT,
		     (vir_bytes) 0, (vir_bytes) &st, (size_t) sizeof(st), D);

  return(r);
}


/*===========================================================================*
 *                             fs_stat					     *
 *===========================================================================*/
int fs_stat()
{
  register int r;              /* return value */
  register struct inode *rip;  /* target inode */

  if ((rip = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
	return(-EINVAL);

  r = stat_inode(rip, fs_m_in.m_source, (cp_grant_id_t) fs_m_in.REQ_GRANT);
  put_inode(rip);		/* release the inode */
  return(r);
}

/*===========================================================================*
 *                             fs_statvfs                                    *
 *===========================================================================*/
int fs_statvfs()
{
  struct statfs st;
  struct super_block *sp;
  int r;

  sp = get_super(fs_dev);

  st.f_bsize =  sp->s_block_size;
  st.f_frsize = sp->s_block_size;
  st.f_blocks = sp->s_blocks_count;
  st.f_bfree = sp->s_free_blocks_count;
  st.f_bavail = sp->s_free_blocks_count - sp->s_r_blocks_count;
  st.f_files = sp->s_inodes_count;
  st.f_ffree = sp->s_free_inodes_count;
  st.f_fsid.val[0] = fs_dev;
  st.f_fsid.val[1] = 0;
  st.f_flags = (sp->s_rd_only == 1 ? ST_RDONLY : 0);
  st.f_flags |= ST_NOTRUNC;
  st.f_namelen = NAME_MAX;

  /* Copy the struct to user space. */
  r = sys_safecopyto(fs_m_in.m_source, fs_m_in.REQ_GRANT, 0, (vir_bytes) &st,
                    (phys_bytes) sizeof(st), D);

  return(r);
}
