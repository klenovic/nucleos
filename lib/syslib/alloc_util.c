/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/syslib.h>

#include <stdlib.h>
#include <nucleos/errno.h>
#include <nucleos/mman.h>
#include <nucleos/sysutil.h>

int sys_umap_data_fb(endpoint_t ep, vir_bytes buf, vir_bytes len, phys_bytes *phys)
{
	int r;

        if((r=sys_umap(ep, VM_D, buf, len, phys)) != 0) {
		if(r != -EINVAL)
			return r;
        	r = sys_umap(ep, D, buf, len, phys);
	}


	return r;
}


void *alloc_contig(size_t len, int flags, phys_bytes *phys)
{
	int r;
	vir_bytes buf;
	int mmapflags = MAP_PREALLOC|MAP_CONTIG|MAP_ANONYMOUS;

	if(flags & AC_LOWER16M)
		mmapflags |= MAP_LOWER16M;
	if(flags & AC_LOWER1M)
		mmapflags |= MAP_LOWER1M;
	if(flags & AC_ALIGN64K)
		mmapflags |= MAP_ALIGN64K;

	/* First try to get memory with mmap. This is gauranteed
	 * to be page-aligned, and we can tell VM it has to be
	 * pre-allocated and contiguous.
	 */
	errno = 0;
	buf = (vir_bytes) mmap(0, len, PROT_READ|PROT_WRITE, mmapflags, -1, 0);

	/* If that failed, maybe we're not running in paged mode.
	 * If that's the case, -ENXIO will be returned.
	 * Memory returned with malloc() will be preallocated and 
	 * contiguous, so fallback on that, and ask for a little extra
	 * so we can page align it ourselves.
	 */
	if(buf == (vir_bytes) MAP_FAILED) {
		u32_t align = 0;
		if(errno != -ENXIO) {
			return NULL;
		}
		if(flags & AC_ALIGN4K)
			align = 4*1024;
		if(flags & AC_ALIGN64K)
			align = 64*1024;
		if(len + align < len)
			return NULL;
		len += align;
		if(!(buf = (vir_bytes) malloc(len))) {
			return NULL;
		}
		if(align)
			buf += align - (buf % align);
	}

	/* Get physical address, if requested. */
        if(phys != NULL && sys_umap_data_fb(ENDPT_SELF, buf, len, phys) != 0)
		panic("alloc_contig.c", "sys_umap_data_fb failed", NO_NUM);

	return (void *) buf;
}

