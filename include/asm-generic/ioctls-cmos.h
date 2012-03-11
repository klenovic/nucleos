/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_cmos.h - CMOS ioctl() command codes.
 */

#ifndef __ASM_GENERIC_IOCTLS_CMOS_H
#define __ASM_GENERIC_IOCTLS_CMOS_H

#include <nucleos/ioctl.h>

#define CIOCGETTIME	_IOR('c', 1, struct tm)
#define CIOCGETTIMEY2K	_IOR('c', 2, struct tm)
#define CIOCSETTIME	_IOW('c', 3, u32_t)
#define CIOCSETTIMEY2K	_IOW('c', 4, u32_t)

#endif /* __ASM_GENERIC_IOCTLS_CMOS_H */

