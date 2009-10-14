/* Some misc functions */

#include "inc.h"
#include <nucleos/fcntl.h>
#include <nucleos/vfsif.h>

/*===========================================================================*
 *				fs_sync					     *
 *===========================================================================*/
int fs_sync()		/* Calling of syncing the filesystem. No action
				 * is taken */
{
  return(0);		/* sync() can't fail */
}
