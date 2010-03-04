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
/* Perform the sync() system call.  No-op on this FS. */

  return(0);		/* sync() can't fail */
}


