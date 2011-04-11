/* Created (MFS based):
 *   February 2010 (Evgeniy Ivanov)
 */

#include "fs.h"
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <servers/fs/ext2/inode.h>
#include <nucleos/vfsif.h>


/*===========================================================================*
 *				fs_utime				     *
 *===========================================================================*/
int fs_utime()
{
  register struct inode *rip;
  register int r;

  /* Temporarily open the file. */
  if( (rip = get_inode(fs_dev, (ino_t) fs_m_in.REQ_INODE_NR)) == NULL)
        return(-EINVAL);

  /* Only the owner of a file or the super_user can change its time. */
  r = 0;
  if(read_only(rip) != 0) r = -EROFS;	/* not even su can touch if R/O */
  if(r == 0) {
	  rip->i_atime = fs_m_in.REQ_ACTIME;
	  rip->i_mtime = fs_m_in.REQ_MODTIME;
	  rip->i_update = CTIME; /* discard any stale ATIME and MTIME flags */
	  rip->i_dirt = DIRTY;
  }

  put_inode(rip);
  return(r);
}
