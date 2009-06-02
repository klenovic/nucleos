/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __KERNEL_DEBUG_H
#define __KERNEL_DEBUG_H

/* This header file defines all debugging constants and macros, and declares
 * some variables. Certain debugging features redefine standard constants
 * and macros. Therefore, this header file should be included after the
 * other kernel headers.
 */

#include <ansi.h>
#include <nucleos/config.h>

#ifdef __KERNEL__
/* Enable prints such as
 *  . send/receive failed due to deadlock or dead source or dead destination
 *  . trap not allowed
 *  . bogus message pointer
 *  . kernel call number not allowed by this process
 *
 * Of course the call still fails, but nothing is printed if these warnings
 * are disabled.
 */
#define DEBUG_ENABLE_IPC_WARNINGS	1
#define DEBUG_STACKTRACE		1
#define DEBUG_TIME_LOCKS		1

/* Runtime sanity checking. */
#define DEBUG_VMASSERT			1
#define DEBUG_SCHED_CHECK		0

#endif /* __KERNEL__ */

#endif /* __KERNEL_DEBUG_H */
