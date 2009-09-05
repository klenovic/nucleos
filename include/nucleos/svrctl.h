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
sys/svrctl.h

Created:	Feb 15, 1994 by Philip Homburg <philip@cs.vu.nl>
*/

#ifndef __NUCLEOS_SVRCTL_H
#define __NUCLEOS_SVRCTL_H

#include <nucleos/types.h>
#include <asm/svrctl.h>

struct mmswapon {
	__u32		offset;		/* Starting offset within file. */
	__u32		size;		/* Size of swap area. */
	char		file[128];	/* Name of swap file/device. */
};

struct svrqueryparam {
	char		*param;		/* Names of parameters to query. */
	size_t		psize;		/* Length of param[]. */
	char		*value;		/* To return values. */
	size_t		vsize;
};

/* A proper system call must be created later. */
#include <nucleos/dmap.h>

struct fssignon {
	dev_t		dev;		/* Device to manage. */
	enum dev_style	style;		/* Management style. */
};

struct fsdevunmap {
	dev_t		dev;		/* Device to unmap. */
};

struct systaskinfo {
	int		proc_nr;	/* Process number of caller. */
};

struct sysgetenv {
	char		*key;		/* Name requested. */
	size_t		keylen;		/* Length of name including \0. */
	char		*val;		/* Buffer for returned data. */
	size_t		vallen;		/* Size of return data buffer. */
};

int svrctl(int _request, void *_data);

#endif /* __NUCLEOS_SVRCTL_H */
