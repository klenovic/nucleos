#include "fs.h"
#include <nucleos/stat.h>
#include <nucleos/string.h>
#include <nucleos/com.h>
#include <nucleos/unistd.h>
#include "buf.h"
#include "inode.h"
#include <nucleos/vfsif.h>

/*===========================================================================*
 *				fs_ftrunc				     *
 *===========================================================================*/
int fs_ftrunc(void)
{
  struct inode *rip;
  off_t start, end;
  int r;
  ino_t inumb;
  
  inumb = fs_m_in.REQ_INODE_NR;

  if( (rip = find_inode(inumb)) == NIL_INODE) return(-EINVAL);

  start = fs_m_in.REQ_TRC_START_LO;
  end = fs_m_in.REQ_TRC_END_LO;

  return truncate_inode(rip, start);
}
    

/*===========================================================================*
 *				truncate_inode				     *
 *===========================================================================*/
int truncate_inode(rip, newsize)
register struct inode *rip;	/* pointer to inode to be truncated */
off_t newsize;			/* inode must become this size */
{
/* Set inode to a certain size, freeing any zones no longer referenced
 * and updating the size in the inode. If the inode is extended, the
 * extra space is a hole that reads as zeroes.
 *
 * Nothing special has to happen to file pointers if inode is opened in
 * O_APPEND mode, as this is different per fd and is checked when 
 * writing is done.
 */
  int scale;

  /* Pipes can shrink, so adjust size to make sure all zones are removed. */
  if(newsize != 0) return(-EINVAL);	/* Only truncate pipes to 0. */
  rip->i_size = newsize;
  
  /* Next correct the inode size. */
  wipe_inode(rip);	/* Pipes can only be truncated to 0. */

  return(0);
}

