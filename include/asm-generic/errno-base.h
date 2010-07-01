/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_ERRNO_BASE_H
#define __ASM_GENERIC_ERRNO_BASE_H

#define EPERM		1   /* operation not permitted */
#define ENOENT		2   /* no such file or directory */
#define ESRCH		3   /* no such process */
#define EINTR		4   /* interrupted function call */
#define EIO		5   /* input/output error */
#define ENXIO		6   /* no such device or address */
#define E2BIG		7   /* arg list too long */
#define ENOEXEC		8   /* exec format error */
#define EBADF		9   /* bad file descriptor */
#define ECHILD		10  /* no child process */
#define EAGAIN		11  /* resource temporarily unavailable */
#define ENOMEM		12  /* not enough space */
#define EACCES		13  /* permission denied */
#define EFAULT		14  /* bad address */
#define ENOTBLK		15  /* Extension: not a block special file */
#define EBUSY		16  /* resource busy */
#define EEXIST		17  /* file exists */
#define EXDEV		18  /* improper link */
#define ENODEV		19  /* no such device */
#define ENOTDIR		20  /* not a directory */
#define EISDIR		21  /* is a directory */
#define EINVAL		22  /* invalid argument */
#define ENFILE		23  /* too many open files in system */
#define EMFILE		24  /* too many open files */
#define ENOTTY		25  /* inappropriate I/O control operation */
#define ETXTBSY		26  /* no longer used */
#define EFBIG		27  /* file too large */
#define ENOSPC		28  /* no space left on device */
#define ESPIPE		29  /* invalid seek */
#define EROFS		30  /* read-only file system */
#define EMLINK		31  /* too many links */
#define EPIPE		32  /* broken pipe */
#define EDOM		33  /* domain error (from ANSI C std) */
#define ERANGE		34  /* result too large (from ANSI C std) */

#endif /* __ASM_GENERIC_ERRNO_BASE_H */
