/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_POSIX_TYPES_H
#define __NUCLEOS_POSIX_TYPES_H

#include <nucleos/stddef.h>

/* Note that POSIX wants the FD_CLEAR(fd,fdsetp) defines to be in
 * <nucleos/time.h> (and thus <nucleos/time.h>) - but this is a more logical
 * place for them. Solved by having dummy defines in <nucleos/time.h>.
 */

/* This many bits fit in an fd_set word. */
#undef __NFDBITS	// FDBITSPERWORD
#define __NFDBITS	(8 * sizeof(unsigned long))

/* Default FD_SETSIZE is OPEN_MAX. */
#undef __FD_SETSIZE	// FD_SETSIZE
#define __FD_SETSIZE	32 /* @nucleos: the value of OPEN_MAX */


#undef __FDSET_LONGS
#define __FDSET_LONGS   (__FD_SETSIZE/__NFDBITS)

#undef __FDELT		//_FD_BITWORD
#define __FDELT(d)      ((d) / __NFDBITS)

/* Bit manipulation macros */
#undef __FDMASK		// _FD_MASK
#define __FDMASK(d)     (1UL << ((d) % __NFDBITS))

typedef struct {
	unsigned long fds_bits[__FDSET_LONGS];
} __kernel_fd_set;

/* Type of a SYSV IPC key */
typedef int __kernel_key_t;
typedef int __kernel_mqd_t;

#include <asm/posix_types.h>

#endif /* __NUCLEOS_POSIX_TYPES_H */
