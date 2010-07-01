/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This is the master header for fs.  It includes some other files
 * and defines the principal constants.
 */
#define VERBOSE		   0    /* show messages during initialization? */

/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/types.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <nucleos/dmap.h>

#include <nucleos/limits.h>
#include <nucleos/errno.h>

#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

#include <servers/mfs/const.h>
#include <servers/mfs/type.h>
#include "proto.h"
#include "glo.h"
