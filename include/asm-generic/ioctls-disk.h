/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_disk.h - Disk ioctl() command codes.	Author: Kees J. Bot
 *								23 Nov 2002
 *
 */

#ifndef __ASM_GENERIC_IOCTLS_DISK_H
#define __ASM_GENERIC_IOCTLS_DISK_H

#include <nucleos/ioctl.h>

#define DIOCSETP	_IOW('d', 3, struct partition)
#define DIOCGETP	_IOR('d', 4, struct partition)
#define DIOCEJECT	_IO ('d', 5)
#define DIOCTIMEOUT	_IORW('d', 6, int)
#define DIOCOPENCT	_IOR('d', 7, int)

#endif /* __ASM_GENERIC_IOCTLS_DISK_H */
