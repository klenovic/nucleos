/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
sys/uio.h

definitions for vector I/O operations
*/

#ifndef _SYS_UIO_H
#define _SYS_UIO_H

/* Open Group Base Specifications Issue 6 (not complete) */

struct iovec
{
	void	*iov_base;
	size_t	iov_len;
};

#if 0
_PROTOTYPE(ssize_t readv, (int _fildes, const struct iovec *_iov,
							int _iovcnt)	);
_PROTOTYPE(ssize_t writev, (int _fildes, const struct iovec *_iov,
							int iovcnt)	);
#endif

#endif /* _SYS_UIO_H */
