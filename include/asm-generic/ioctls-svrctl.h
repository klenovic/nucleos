/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
sys/svrctl.h

Created:	Feb 15, 1994 by Philip Homburg <philip@cs.vu.nl>
*/

#ifndef __ASM_GENERIC_IOCTLS_SVRCTL_H
#define __ASM_GENERIC_IOCTLS_SVRCTL_H

/* Server control commands have the same encoding as the commands for ioctls. */
#include <nucleos/ioctl.h>

/* MM controls. */
#define MMSIGNON	_IO ('M',  4)
#define MMSWAPON	_IOW('M',  5, struct mmswapon)
#define MMSWAPOFF	_IO ('M',  6)
#define MMGETPARAM	_IOW('M',  5, struct sysgetenv)
#define MMSETPARAM	_IOR('M',  7, struct sysgetenv)

/* FS controls. */
#define FSSIGNON	_IOW('F',  2, struct fssignon)
#define FSDEVUNMAP	_IOW('F',  6, struct fsdevunmap)

/* Kernel controls. */
#define SYSSENDMASK	_IO ('S',  4)
#define SYSSIGNON	_IOR('S',  2, struct systaskinfo)
#define SYSGETENV	_IOW('S',  1, struct sysgetenv)

#endif /* __ASM_GENERIC_IOCTLS_SVRCTL_H */
