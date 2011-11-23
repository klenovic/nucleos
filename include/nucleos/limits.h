/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/limits.h> header defines some basic sizes, both of the language types 
 * (e.g., the number of bits in an integer), and of the operating system (e.g.
 * the number of characters in a file name.
 */

#ifndef _NUCLEOS_LIMITS_H
#define _NUCLEOS_LIMITS_H

/* Minimum sizes required by the POSIX P1003.1 standard (Table 2-3). */
#define _POSIX_ARG_MAX		4096	/* exec() may have 4K worth of args */
#define _POSIX_CHILD_MAX	6	/* a process may have 6 children */
#define _POSIX_LINK_MAX		8	/* a file may have 8 links */
#define _POSIX_MAX_CANON	255	/* size of the canonical input queue */
#define _POSIX_MAX_INPUT	255	/* you can type 255 chars ahead */
#define _POSIX_NAME_MAX		60	/* max. file name length; @nucleos: the value of DIRSIZ */
#define _POSIX_NGROUPS_MAX	8	/* supplementary group IDs are optional */
#define _POSIX_OPEN_MAX		16	/* a process may have 16 files open */
#define _POSIX_PATH_MAX		255	/* a pathname may contain 255 chars */
#define _POSIX_PIPE_BUF		512	/* pipes writes of 512 bytes must be atomic */
#define _POSIX_STREAM_MAX	8	/* at least 8 FILEs can be open at once */
#define _POSIX_TZNAME_MAX	3	/* time zone names can be at least 3 chars */
#define _POSIX_SSIZE_MAX	32767	/* read() must support 32767 byte reads */
#define _POSIX_SYMLOOP_MAX	8	/* The number of symbolic links that can be
					 * traversed in the resolution of a pathname
					 * in the absence of a loop.
					 */
#define _POSIX_SYMLINK_MAX	255	/* # number of bytes in a symbolic link */

/* Values actually implemented by MINIX (Tables 2-4, 2-5, 2-6, and 2-7). */
/* Some of these old names had better be defined when not POSIX. */
#define _NO_LIMIT		100	/* arbitrary number; limit not enforced */

#define NGROUPS_MAX	8		/* supplemental group IDs not available */
#define ARG_MAX		262144		/* # bytes of args + environ for exec() */
#define CHILD_MAX	_NO_LIMIT	/* MINIX does not limit children */
#define OPEN_MAX	32		/* # open files a process may have */

#define LINK_MAX	32767  /* # links a file may have (V2 or better file system)
				* @nucleos: the value of SHORT_MAX
				*/

#define MAX_CANON	255	/* size of the canonical input queue */
#define MAX_INPUT	255	/* size of the type-ahead buffer */
#define NAME_MAX	60 	/* # chars in a file name; @nucleos: the value of DIRSIZ */
#define PATH_MAX	255	/* # chars in a path name */
#define PIPE_BUF	7168	/* # bytes in atomic write to a pipe */
#define STREAM_MAX	20	/* must be the same as FOPEN_MAX in stdio.h */
#define TZNAME_MAX	3	/* maximum bytes in a time zone name is 3 */
#define SSIZE_MAX	32767	/* max defined byte count for read() */
#define SYMLINK_MAX	1024	/* # number of bytes in a symbolic link */
#define SYMLOOP_MAX	16	/* maximum number of symbolic links that can
				 * be reliably traversed in the resolution of
				 * a pathname in the absence of a loop.
				 */

#endif /* _NUCLEOS_LIMITS_H */
