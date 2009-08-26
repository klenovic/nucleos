/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_FS_H
#define __SERVERS_VFS_FS_H

#include <nucleos/stringify.h>

#define APPNAME		__APPNAME__
#define APPVERSION	__APPVERSION__
#define APPTYPE		__APPTYPE__

#ifdef CONFIG_DEBUG_VFS
#define app_dbg(format, args...) \
	printf("D:%s:%s:%s:%d:" format, __stringify(APPTYPE), __stringify(APPNAME), \
					__FUNCTION__, __LINE__, ##args);
#else
#define app_dbg(format, args...)
#endif

#define app_info(format, args...) \
	printf("I:%s:%s:" format, __stringify(APPTYPE), __stringify(APPNAME), ##args);

#define app_warn(format, args...) \
	printf("W:%s:%s:" format, __stringify(APPTYPE), __stringify(APPNAME), ##args);

#define app_err(format, args...) \
	printf("E:%s:%s:%s:%d:" format, __stringify(APPTYPE), __stringify(APPNAME), \
					__FUNCTION__, __LINE__, ##args);

/* This is the master header for fs.  It includes some other files
 * and defines the principal constants.
 */
#define DO_SANITYCHECKS	   0

#if DO_SANITYCHECKS
#define SANITYCHECK do { 			\
	if(!check_vrefs() || !check_pipe()) {				\
	   printf("VFS:%s:%d: call_nr %d who_e %d\n", \
			__FILE__, __LINE__, call_nr, who_e); 	\
	   panic(__FILE__, "sanity check failed", NO_NUM);	\
	}							\
} while(0)
#else
#define SANITYCHECK
#endif

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
#include "dmap.h"
#include "proto.h"
#include "glo.h"

#endif /* __SERVERS_VFS_FS_H */

