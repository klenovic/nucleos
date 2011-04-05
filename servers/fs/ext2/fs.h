/* This is the master header for fs.  It includes some other files
 * and defines the principal constants.
 */

#ifndef EXT2_FS_H
#define EXT2_FS_H

/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/types.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <nucleos/dmap.h>

#include <nucleos/limits.h>
#include <nucleos/errno.h>

#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

#include "const.h"
#include "type.h"
#include "proto.h"
#include "glo.h"

#endif /* EXT2_FS_H */
