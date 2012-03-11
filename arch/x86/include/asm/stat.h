/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/stat.h> header defines a struct that is used in the stat() and
 * fstat functions.  The information in this struct comes from the i-node of
 * some file.  These calls are the only approved way to inspect i-nodes.
 */

#ifndef __ASM_X86_STAT_H
#define __ASM_X86_STAT_H

#include <nucleos/types.h>

#ifdef __i386__
struct stat {
	short		st_dev;		/* major/minor device number */
	unsigned long	st_ino;		/* i-node number */
	unsigned short	st_mode;	/* file mode, protection bits, etc. */
	short		st_nlink;	/* # links; TEMPORARY HACK: should be nlink_t*/
	short		st_uid;		/* uid of the file's owner */
	short		st_gid;		/* gid; TEMPORARY HACK: should be gid_t */
	short		st_rdev;	/* device ID (if special file) */
	long		st_size;	/* file size */
	unsigned long	st_blksize;	/* blocksize for file system I/O */
	unsigned long	st_blocks;	/* number of 512B blocks allocated */
	unsigned long	st_atime;	/* time of last access */
	unsigned long	st_atime_nsec;	/* time of last access (nanosec) */
	unsigned long	st_mtime;	/* time of last data modification */
	unsigned long	st_mtime_nsec;	/* time of last data modification (nanosec) */
	unsigned long	st_ctime;	/* time of last file status change */
	unsigned long	st_ctime_nsec;	/* time of last file status change (nanosec) */
	unsigned long	__unused4;
	unsigned long	__unused5;
};
#else /* __i386__*/

/* something else */

#endif

#endif /* __ASM_X86_STAT_H */
