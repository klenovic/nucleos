/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_FS_MINIXFS_INODE_H
#define __SERVERS_FS_MINIXFS_INODE_H

/* Inode table.  This table holds inodes that are currently in use.  In some
 * cases they have been opened by an open() or creat() system call, in other
 * cases the file system itself needs the inode for one reason or another,
 * such as to search a directory for a path name.
 * The first part of the struct holds fields that are present on the
 * disk; the second part holds fields not present on the disk.
 * The disk inode part is also declared in "type.h" as 'd1_inode' for V1
 * file systems and 'd2_inode' for V2 file systems.
 *
 * Updates:
 * 2007-01-06: jfdsmit@gmail.com added i_zsearch
 */
#include <nucleos/queue.h>
#include <servers/fs/minixfs/type.h>

struct minix_inode {
	mode_t i_mode;		/* file type, protection, etc. */
	nlink_t i_nlinks;		/* how many links to this file */
	uid_t i_uid;			/* user id of the file's owner */
	gid_t i_gid;			/* group number */
	off_t i_size;			/* current file size in bytes */
	time_t i_atime;		/* time of last access (V2 only) */
	time_t i_mtime;		/* when was file data last changed */
	time_t i_ctime;		/* when was inode itself changed (V2 only)*/
	zone_t i_zone[V2_NR_TZONES]; /* zone numbers for direct, ind, and dbl ind */

#if defined(__KERNEL__) || defined(__UKERNEL__)
	/* The following items are not present on the disk. */
	dev_t i_dev;			/* which device is the inode on */
	ino_t i_num;			/* inode number on its (minor) device */
	int i_count;			/* # times inode used; 0 means slot is free */
	int i_ndzones;		/* # direct zones (Vx_NR_DZONES) */
	int i_nindirs;		/* # indirect zones per indirect block */
	struct minix_super_block *i_sp;	/* pointer to super block for inode's device */
	char i_dirt;			/* CLEAN or DIRTY */
	u32 i_zsearch;		/* where to start search for new zones */

	char i_mountpoint;		/* true if mounted on */

	char i_seek;			/* set on LSEEK, cleared on READ/WRITE */
	char i_update;		/* the ATIME, CTIME, and MTIME bits are here */

	LIST_ENTRY(minix_inode) i_hash;     /* hash list */
	TAILQ_ENTRY(minix_inode) i_unused;  /* free and unused list */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
extern struct minix_inode inode[];

/* list of unused/free inodes */ 
extern TAILQ_HEAD(unused_inodes_t, minix_inode)  unused_inodes;

/* inode hashtable */
extern LIST_HEAD(inodelist, minix_inode) hash_inodes[INODE_HASH_SIZE];

extern unsigned int inode_cache_hit;
extern unsigned int inode_cache_miss;

#define NIL_INODE	(struct minix_inode *)0	/* indicates absence of inode slot */

/* Field values.  Note that CLEAN and DIRTY are defined in "const.h" */
#define NO_SEEK		0	/* i_seek = NO_SEEK if last op was not SEEK */
#define ISEEK		1	/* i_seek = ISEEK if last op was SEEK */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __SERVERS_FS_MINIXFS_INODE_H */
