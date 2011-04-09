/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __PM_PM_H
#define __PM_PM_H

/* This is the master header for PM.  It includes some other files
 * and defines the principal constants.
 */
/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/types.h>
#include <nucleos/const.h>
#include <nucleos/type.h>

#include <nucleos/fcntl.h>
#include <nucleos/unistd.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

#include <nucleos/limits.h>
#include <nucleos/errno.h>

#include <servers/pm/const.h>
#include "type.h"
#include "proto.h"
#include "glo.h"

#endif /* __PM_PM_H */
