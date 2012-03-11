/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	dirent.h - Declarations for directory reading routines.
 *							Author: Kees J. Bot
 *								24 Apr 1989
 * 
 * Note: The V7 format directory entries used under Minix must be transformed
 * into a struct dirent with a d_name of at least 15 characters.  Given that
 * we have to transform V7 entries anyhow it is little trouble to let the
 * routines understand the so-called "flex" directory format too.
 */

#ifndef __DIRENT_H
#define __DIRENT_H

#ifndef _TYPES_H
#include "sys/types.h"
#endif

#include "sys/dir.h"

/* _fl_direct is a flexible directory entry.  Actually it's a union of 8
 * characters and the 3 fields defined below. 
 */

/* Flexible directory entry: */
struct _fl_direct {		/* First slot in an entry */
	MNX(ino_t)		d_ino;
	unsigned char	d_extent;
	char		d_name[3];  /* two characters for the shortest name */
};

	/* Name of length len needs _EXTENT(len) extra slots. */
#define _EXTENT(len)	(((len) + 5) >> 3)

/* Version 7 directory entry: */
struct _v7_direct {		
	MNX(ino_t)		d_ino;
	char		d_name[DIRSIZ];
};

/* The block size must be at least 1024 bytes, because otherwise
 * the superblock (at 1024 bytes) overlaps with other filesystem data.
 */
#define _MIN_BLOCK_SIZE		 1024

/* The below is allocated in some parts of the system as the largest
 * a filesystem block can be. For instance, the boot monitor allocates
 * 3 of these blocks and has to fit within 64kB, so this can't be
 * increased without taking that into account.
 */
#define _MAX_BLOCK_SIZE		 4096

/* This is the block size for the fixed versions of the filesystem (V1/V2) */
#define _STATIC_BLOCK_SIZE	1024

#define _STATIC_FLEX_PER_BLOCK (_STATIC_BLOCK_SIZE/sizeof(struct _fl_direct))
#define _FLEX_PER_V7 (_EXTENT(DIRSIZ) + 1)
#define _FLEX_PER_BLOCK (_STATIC_BLOCK_SIZE/sizeof(struct _fl_direct))

/* Definitions for the directory(3) routines: */
typedef struct {
	char		_fd;	/* Filedescriptor of open directory */
	unsigned	_count;	/* This many bytes in _buf */
	unsigned	_pos;	/* Position in _buf */
	char		 _buf[_MAX_BLOCK_SIZE]; /* The size does not really
						 * matter as long as the
						 * buffer is big enough 
						 * to contain at least one 
						 * entry.
						 */
} MNX(DIR);

#define _DIRENT_NAME_LEN 61

struct MNX(dirent) {              /* Largest entry (8 slots) */
	MNX(ino_t)		d_ino;		/* I-node number */
	MNX(off_t) 		d_off;		/* Offset in directory */
	unsigned short	d_reclen;	/* Length of this record */
	char		d_name[1];	/* Null terminated name */
};

#endif /* __DIRENT_H */
