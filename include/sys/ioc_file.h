/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_file.h - File ioctl() command codes.
 */

#ifndef _SYS_IOC_FILE_H
#define _SYS_IOC_FILE_H

#include <nucleos/ioctl.h>

#define FIONREAD	_IOR('f', 1, int)

#endif /* _SYS_IOC_FILE_H */
