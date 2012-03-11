/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_FS_MINIXFS_TYPE_H
#define __SERVERS_FS_MINIXFS_TYPE_H

#include <nucleos/types.h>
#include <servers/fs/minixfs/const.h>

/* Declaration of the V1 inode as it is on the disk (not in core). */
typedef struct {		/* V1.x disk inode */
	__u16 d1_mode;		/* file type, protection, etc. */
	__s16 d1_uid;		/* user id of the file's owner */
	__s32 d1_size;		/* current file size in bytes */
	__s32 d1_mtime;		/* when was file data last changed */
	__u8 d1_gid;		/* group number */
	__u8 d1_nlinks;		/* how many links to this file */
	__u16 d1_zone[V1_NR_TZONES];	/* block nums for direct, ind, and dbl ind */
} d1_inode;

/* Declaration of the V2 inode as it is on the disk (not in core). */
typedef struct {		/* V2.x disk inode */
	__u16 d2_mode;		/* file type, protection, etc. */
	__u16 d2_nlinks;	/* how many links to this file. HACK! */
	__s16 d2_uid;		/* user id of the file's owner. */
	__u16 d2_gid;		/* group number HACK! */
	__s32 d2_size;		/* current file size in bytes */
	__s32 d2_atime;		/* when was file data last accessed */
	__s32 d2_mtime;		/* when was file data last changed */
	__s32 d2_ctime;		/* when was inode data last changed */
	__u32 d2_zone[V2_NR_TZONES];	/* block nums for direct, ind, and dbl ind */
} d2_inode;

#if defined (__KERNEL__) || defined(__UKERNEL__)
typedef u32 zone_t;	/* zone number */
typedef u32 block_t;	/* block number */
typedef u16 zone1_t;	/* zone number for V1 file systems */

struct buf {
	/* Data portion of the buffer. */
	union fsdata_u *bp;

	/* Header portion of the buffer. */
	struct buf *b_next;	/* used to link all free bufs in a chain */
	struct buf *b_prev;	/* used to link all free bufs the other way */
	struct buf *b_hash;	/* used to link bufs on hash chains */
	block_t b_blocknr;	/* block number of its (minor) device */
	dev_t b_dev;		/* major | minor device where block resides */
	char b_dirt;		/* CLEAN or DIRTY */
	char b_count;		/* number of users of this buffer */
	int b_bytes;		/* Number of bytes allocated in bp */
};
#endif /* defined (__KERNEL__) || defined(__UKERNEL__) */

#endif /* __SERVERS_FS_MINIXFS_TYPE_H */
