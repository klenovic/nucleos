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

#ifndef _NUCLEOS_DIRENT_H
#define _NUCLEOS_DIRENT_H

#include <nucleos/types.h>

struct dirent {		/* Largest entry (8 slots) */
	ino_t		d_ino;		/* I-node number */
	off_t 		d_off;		/* Offset in directory */
	unsigned short	d_reclen;	/* Length of this record */
	char		d_name[1];	/* Null terminated name */
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
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
} DIR;

/* Function Prototypes. */
int closedir(DIR *_dirp);
DIR *opendir(const char *_dirname);
struct dirent *readdir(DIR *_dirp);
void rewinddir(DIR *_dirp);

int seekdir(DIR *_dirp, off_t _loc);
off_t telldir(DIR *_dirp);

#define dirfd(dirp)	((dirp)->_fd)

int getdents(int _fildes, struct dirent *_buf, size_t _nbyte);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* _NUCLEOS_DIRENT_H */
