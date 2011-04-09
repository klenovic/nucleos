/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
minix/swap.h

Defines the super block of swap partitions and some useful constants.

Created:	Aug 2, 1992 by Philip Homburg
*/

#ifndef __NUCLEOS_SWAP_H
#define __NUCLEOS_SWAP_H

/* Two possible layouts for a partition with swapspace:
 *
 *	Sector		Swap partition		VFS_PROC_NR+swap partition
 *
 *       0 - 1		bootblock		bootblock
 *	     2		swap header		VFS_PROC_NR header
 *	     3		blank			swap header
 *	 4 - m		swapspace		file system
 *	m+1 - n		-			swapspace
 */
 
#define SWAP_MAGIC0	0x9D
#define SWAP_MAGIC1	0xC3
#define SWAP_MAGIC2	0x01
#define SWAP_MAGIC3	0x82

typedef struct swap_hdr
{
	u8_t sh_magic[4];
	u8_t sh_version;
	u8_t sh_dummy[3];
	u32_t sh_offset;
	u32_t sh_swapsize;
	i32_t sh_priority;
} swap_hdr_t;

#define SWAP_BOOTOFF	 1024
#define SWAP_OFFSET	 2048
#define OPTSWAP_BOOTOFF	(1024+512)
#define SH_VERSION	    1
#define SH_PRIORITY	    0

#endif /* __NUCLEOS_SWAP_H */
