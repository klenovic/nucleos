/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Data for fstatfs() call. */

#ifndef __NUCLEOS_STATFS_H
#define __NUCLEOS_STATFS_H

#include <nucleos/types.h>
#include <asm/statfs.h>

struct kstatfs {
	long f_type;
	long f_bsize;
	u64 f_blocks;
	u64 f_bfree;
	u64 f_bavail;
	u64 f_files;
	u64 f_ffree;
	__kernel_fsid_t f_fsid;
	long f_namelen;
	long f_frsize;
	long f_spare[5];
};

/* Possible values for statvfs->f_flag */
#define ST_RDONLY	0x001	/* Read-only file system */
#define ST_NOSUID	0x002	/* Does not support the semantics of the
				 * ST_ISUID and ST_ISGID file mode bits. */
#define ST_NOTRUNC	0x004	/* File system does not truncate file names
				 * longer than NAME_MAX */

#if defined(__KERNEL__) || defined(__UKERNEL__)
int fstatfs(int fd, struct statfs *st);
int statvfs(const char *path, struct statfs *st);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_STATFS_H */
