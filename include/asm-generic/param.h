/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_PARAM_H
#define __ASM_GENERIC_PARAM_H

#if defined(__KERNEL__) || defined(__UKERNEL__)
#define HZ		CONFIG_HZ	/* Internal kernel timer frequency */
#define USER_HZ		60
#define CLOCKS_PER_SEC	(USER_HZ)	/* in "ticks" */
#endif

#ifndef HZ
#define HZ	60
#endif

#ifndef EXEC_PAGESIZE
#define EXEC_PAGESIZE	4096
#endif

#define MAXHOSTNAMELEN  256	/* max hostname size */

#endif /* __ASM_GENERIC_PARAM_H */
