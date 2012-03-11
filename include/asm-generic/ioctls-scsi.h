/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	sys/ioc_scsi.h - SCSI ioctl() command codes.	Author: Kees J. Bot
 *								23 Nov 2002
 *
 */
#ifndef __ASM_GENERIC_IOCTLS_SCSI_H
#define __ASM_GENERIC_IOCTLS_SCSI_H

#include <nucleos/ioctl.h>

#define SCIOCCMD	_IOW('S', 1, struct scsicmd)

#endif /* __ASM_GENERIC_IOCTLS_SCSI_H */
