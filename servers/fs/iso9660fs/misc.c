#include "inc.h"
#include <nucleos/fcntl.h>
#include <nucleos/vfsif.h>

/*===========================================================================*
 *				fs_sync					     *
 *===========================================================================*/
int fs_sync()
{
  /* Always mounted read only, so nothing to sync */
  return(0);		/* sync() can't fail */
}
