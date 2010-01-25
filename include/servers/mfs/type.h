/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_MFS_TYPE_H
#define __SERVERS_MFS_TYPE_H

/* Declaration of the V1 inode as it is on the disk (not in core). */
typedef struct {		/* V1.x disk inode */
	u16_t d1_mode;		/* file type, protection, etc. */
	i16_t d1_uid;		/* user id of the file's owner */
	i32_t d1_size;		/* current file size in bytes */
	i32_t d1_mtime;		/* when was file data last changed */
	u8_t d1_gid;		/* group number */
	u8_t d1_nlinks;		/* how many links to this file */
	u16_t d1_zone[V1_NR_TZONES];	/* block nums for direct, ind, and dbl ind */
} d1_inode;

/* Declaration of the V2 inode as it is on the disk (not in core). */
typedef struct {		/* V2.x disk inode */
	u16_t d2_mode;		/* file type, protection, etc. */
	u16_t d2_nlinks;	/* how many links to this file. HACK! */
	i16_t d2_uid;		/* user id of the file's owner. */
	u16_t d2_gid;		/* group number HACK! */
	i32_t d2_size;		/* current file size in bytes */
	i32_t d2_atime;		/* when was file data last accessed */
	i32_t d2_mtime;		/* when was file data last changed */
	i32_t d2_ctime;		/* when was inode data last changed */
	zone_t d2_zone[V2_NR_TZONES];	/* block nums for direct, ind, and dbl ind */
} d2_inode;

#if defined (__KERNEL__) || defined(__UKERNEL__)
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

#endif /* __SERVERS_MFS_TYPE_H */
