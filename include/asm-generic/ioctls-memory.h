/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_memory.h - Memory ioctl() command codes.
 *							Author: Kees J. Bot
 *								23 Nov 2002
 *
 */

#ifndef __ASM_GENERIC_IOCTLS_MEMORY_H
#define __ASM_GENERIC_IOCTLS_MEMORY_H

#include <nucleos/ioctl.h>

#define MIOCRAMSIZE	_IOW('m', 3, u32_t)
#define MIOCMAP		_IOW('m', 4, struct mapreq)
#define MIOCUNMAP	_IOW('m', 5, struct mapreq)

#endif /* __ASM_GENERIC_IOCTLS_MEMORY_H */
