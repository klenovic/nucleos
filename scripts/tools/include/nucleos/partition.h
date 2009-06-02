/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	minix/partition.h				Author: Kees J. Bot
 *								7 Dec 1995
 * Place of a partition on disk and the disk geometry,
 * for use with the DIOCGETP and DIOCSETP ioctl's.
 */
#ifndef _MINIX__PARTITION_H
#define _MINIX__PARTITION_H

#ifndef _TYPES_H
#include "../sys/types.h"
#endif

struct MNX(partition) {
  MNX(u64_t) base;		/* byte offset to the partition start */
  MNX(u64_t) size;		/* number of bytes in the partition */
  unsigned cylinders;	/* disk geometry */
  unsigned heads;
  unsigned sectors;
};

#endif /* _MINIX__PARTITION_H */
