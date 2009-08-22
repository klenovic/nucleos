/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#define _SYSTEM            1	/* tell headers that this is the kernel */

/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/types.h>		/* general system types */
#include <nucleos/const.h>	/* MINIX specific constants */
#include <nucleos/type.h>		/* MINIX specific types, e.g. message */
#include <nucleos/ipc.h>		/* MINIX run-time system */
#include <timers.h>		/* watchdog timer management */
#include <nucleos/errno.h>		/* return codes and error numbers */

/* Important kernel header files. */
#include <kernel/const.h>		/* constants, MUST be second */
#include <kernel/type.h>		/* type definitions, MUST be third */
#include <kernel/proto.h>		/* function prototypes */
#include <kernel/glo.h>			/* global variables */
#include <nucleos/ipc.h>		/* IPC constants */
#include <kernel/profile.h>		/* system profiling */

#endif /* __KERNEL_KERNEL_H */
