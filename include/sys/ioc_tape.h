/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_tape.h - Magnetic Tape ioctl() command codes.
 *							Author: Kees J. Bot
 *								23 Nov 2002
 *
 */

#ifndef _S_I_TAPE_H
#define _S_I_TAPE_H

#include <nucleos/ioctl.h>

#define MTIOCTOP	_IOW('M', 1, struct mtop)
#define MTIOCGET	_IOR('M', 2, struct mtget)

#endif /* _S_I_TAPE_H */
