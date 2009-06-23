/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_PM_PM_H
#define __SERVERS_PM_PM_H

/* This is the master header for PM.  It includes some other files
 * and defines the principal constants.
 */
#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1	/* tell headers that this is the kernel */

/* The following are so basic, all the *.c files get them automatically. */
#include <ansi.h>		/* MUST be second */
#include <sys/types.h>
#include <nucleos/const.h>
#include <nucleos/type.h>

#include <fcntl.h>
#include <unistd.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

#include <limits.h>
#include <errno.h>

#include "const.h"
#include "type.h"
#include "proto.h"
#include "glo.h"

#endif /*  __SERVERS_PM_PM_H */
