/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_MFS_SUPER_H
#define __SERVERS_MFS_SUPER_H

#include <nucleos/types.h>
#include <servers/mfs/type.h>

/* Super block table.  The root file system and every mounted file system
 * has an entry here.  The entry holds information about the sizes of the bit
 * maps and inodes.  The s_ninodes field gives the number of inodes available
 * for files and directories, including the root directory.  Inode 0 is 
 * on the disk, but not used.  Thus s_ninodes = 4 means that 5 bits will be
 * used in the bit map, bit 0, which is always 1 and not used, and bits 1-4
 * for files and directories.  The disk layout is:
 *
 *    Item        # blocks
 *    boot block      1
 *    super block     1    (offset 1kB)
 *    inode map     s_imap_blocks
 *    zone map      s_zmap_blocks
 *    inodes        (s_ninodes + 'inodes per block' - 1)/'inodes per block'
 *    unused        whatever is needed to fill out the current zone
 *    data zones    (s_zones - s_firstdatazone) << s_log_zone_size
 *
 * A minix3_super_block slot is free if s_dev == NO_DEV. 
 */

struct minix3_super_block {
	__u32 s_ninodes;	/* # usable inodes on the minor device */
	__u16 s_nzones;		/* total device size, including bit maps etc */
	__s16 s_imap_blocks;	/* # of blocks used by inode bit map */
	__s16 s_zmap_blocks;	/* # of blocks used by zone bit map */
	__u16 s_firstdatazone;	/* number of first data zone */
	__s16 s_log_zone_size;	/* log2 of blocks/zone */
	__s16 s_pad;		/* try to avoid compiler-dependent padding */
	__s32 s_max_size;	/* maximum file size on this device */
	__u32 s_zones;		/* number of zones (replaces s_nzones in V2) */
	__s16 s_magic;		/* magic number to recognize super-blocks */

	/* The following items are valid on disk only for V3 and above */

	/* The block size in bytes. Minimum MIN_BLOCK SIZE. SECTOR_SIZE
	 * multiple. If V1 or V2 filesystem, this should be
	 * initialised to STATIC_BLOCK_SIZE.
	 */
	__s16 s_pad2;		/* try to avoid compiler-dependent padding */
	__u16 s_block_size;	/* block size in bytes. */
	__s8 s_disk_version;	/* filesystem format sub-version */

	/* The following items are only used when the minix3_super_block is in memory. */
	/*struct inode *s_isup;*/	/* inode for root dir of mounted file sys */
	/*struct inode *s_imount;*/	/* inode mounted on */
	__u32 s_inodes_per_block;	/* precalculated from magic number */
	__s16 s_dev;			/* whose super block is this? */
	__s32 s_rd_only;		/* set to 1 iff file sys mounted read only */
	__s32 s_native;			/* set to 1 iff not byte swapped file system */
	__s32 s_version;		/* file system version, zero means bad magic */
	__s32 s_ndzones;		/* # direct zones in an inode */
	__s32 s_nindirs;		/* # indirect zones per indirect block */
	__u32 s_isearch;		/* inodes below this bit number are in use */
	__u32 s_zsearch;		/* all zones below this bit number are in use*/
	__s8 s_is_root;
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
extern struct minix3_super_block superblock;

#define NIL_SUPER	(struct minix3_super_block *) 0
#define IMAP		0	/* operating on the inode bit map */
#define ZMAP		1	/* operating on the zone bit map */

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __SERVERS_MFS_SUPER_H */
