/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
	dev_t st_dev;		/* major/minor device number */
	ino_t st_ino;		/* i-node number */
	mode_t st_mode;		/* file mode, protection bits, etc. */
	short int st_nlink;	/* # links; TEMPORARY HACK: should be nlink_t*/
	uid_t st_uid;		/* uid of the file's owner */
	short int st_gid;	/* gid; TEMPORARY HACK: should be gid_t */
	dev_t st_rdev;		/* device ID (if special file) */
	off_t st_size;		/* file size */
	time_t st_atime;	/* time of last access */
	time_t st_mtime;	/* time of last data modification */
	time_t st_ctime;	/* time of last file status change */
};
#else /* __i386__*/

/* something else */

#endif

#endif /* __ASM_X86_STAT_H */
