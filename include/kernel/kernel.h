/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __KERNEL_KERNEL_H
#define __KERNEL_KERNEL_H

/* This is the master header for the kernel.  It includes some other files
 * and defines the principal constants.
 */
/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/types.h>	/* general system types */
#include <nucleos/const.h>	/* Nucleos specific constants */
#include <nucleos/type.h>	/* Nucleos specific types, e.g. message */
#include <nucleos/kipc.h>	/* Nucleos run-time system */
#include <nucleos/timer.h>	/* watchdog timer management */
#include <nucleos/errno.h>	/* return codes and error numbers */

/* Important kernel header files. */
#include <kernel/const.h>	/* constants */
#include <kernel/types.h>	/* type definitions */
#include <kernel/proto.h>	/* function prototypes */
#include <kernel/glo.h>		/* global variables */
#include <kernel/profile.h>	/* system profiling */
#include <nucleos/kipc.h>	/* IPC constants */
#include <nucleos/sysutil.h>	/* utility library functions */

#endif /* __KERNEL_KERNEL_H */
