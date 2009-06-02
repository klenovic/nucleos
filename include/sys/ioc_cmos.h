/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_cmos.h - CMOS ioctl() command codes.
 */

#ifndef _S_I_CMOS_H
#define _S_I_CMOS_H

#include <nucleos/ioctl.h>

#define CIOCGETTIME	_IOR('c', 1, struct tm)
#define CIOCGETTIMEY2K	_IOR('c', 2, struct tm)
#define CIOCSETTIME	_IOW('c', 3, u32_t)
#define CIOCSETTIMEY2K	_IOW('c', 4, u32_t)

#endif /* _S_I_CMOS_H */

