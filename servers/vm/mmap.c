/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#define _SYSTEM 1

#define VERBOSE 0

#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/config.h>
#include <nucleos/const.h>
#include <server/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/ipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/safecopies.h>

#include <sys/mman.h>

#include <errno.h>
#include <string.h>
#include <env.h>
#include <stdio.h>
#include <fcntl.h>
#include <asm/server/vm/memory.h>

#include "glo.h"
#include "proto.h"
#include "util.h"
#include "region.h"

/*===========================================================================*
 *				do_mmap			     		     *
 *===========================================================================*/
PUBLIC int do_mmap(message *m)
{
	int r, n;
	struct vmproc *vmp;
	int mfflags = 0;
	struct vir_region *vr = NULL;

	if((r=vm_isokendpt(m->m_source, &n)) != OK) {
		vm_panic("do_mmap: message from strange source", m->m_source);
	}

	vmp = &vmproc[n];

	if(m->VMM_FLAGS & MAP_LOWER16M)
		printf("VM: warning for %d: MAP_LOWER16M not implemented\n",
			m->m_source);

	if(!(vmp->vm_flags & VMF_HASPT))
		return ENXIO;

	if(m->VMM_FD == -1 || (m->VMM_FLAGS & MAP_ANON)) {
		int s;
		vir_bytes v;
		u32_t vrflags = VR_ANON | VR_WRITABLE;
		size_t len = (vir_bytes) m->VMM_LEN;

		if(m->VMM_FD != -1) {
			return EINVAL;
		}

		if(m->VMM_FLAGS & MAP_CONTIG) mfflags |= MF_CONTIG;
		if(m->VMM_FLAGS & MAP_PREALLOC) mfflags |= MF_PREALLOC;
		if(m->VMM_FLAGS & MAP_ALIGN64K) vrflags |= VR_PHYS64K;

		if(len % VM_PAGE_SIZE)
			len += VM_PAGE_SIZE - (len % VM_PAGE_SIZE);

		if(!(vr = map_page_region(vmp,
			arch_vir2map(vmp, vmp->vm_stacktop), VM_DATATOP, len, MAP_NONE,
			vrflags, mfflags))) {
			return ENOMEM;
		}
	} else {
		return ENOSYS;
	}

	/* Return mapping, as seen from process. */
	vm_assert(vr);
	m->VMM_RETADDR = arch_map2vir(vmp, vr->vaddr);

	return OK;
}

/*===========================================================================*
 *				do_map_phys		     		     *
 *===========================================================================*/
PUBLIC int do_map_phys(message *m)
{
	int r, n;
	struct vmproc *vmp;
	endpoint_t target;
	struct vir_region *vr;
	vir_bytes len;

	target = m->VMMP_EP;
	if(target == SELF)
		target = m->m_source;

	if((r=vm_isokendpt(target, &n)) != OK)
		return EINVAL;

	vmp = &vmproc[n];

	if(!(vmp->vm_flags & VMF_HASPT))
		return ENXIO;

	len = m->VMMP_LEN;
	if(len % VM_PAGE_SIZE)
		len += VM_PAGE_SIZE - (len % VM_PAGE_SIZE);

	if(!(vr = map_page_region(vmp, arch_vir2map(vmp, vmp->vm_stacktop),
		VM_DATATOP, len, (vir_bytes)m->VMMP_PHADDR,
		VR_DIRECT | VR_NOPF | VR_WRITABLE, 0))) {
		return ENOMEM;
	}

	m->VMMP_VADDR_REPLY = (void *) arch_map2vir(vmp, vr->vaddr);

	return OK;
}

/*===========================================================================*
 *				do_unmap_phys		     		     *
 *===========================================================================*/
PUBLIC int do_unmap_phys(message *m)
{
	int r, n;
	struct vmproc *vmp;
	endpoint_t target;
	struct vir_region *region;

	target = m->VMUP_EP;
	if(target == SELF)
		target = m->m_source;

	if((r=vm_isokendpt(target, &n)) != OK)
		return EINVAL;

	vmp = &vmproc[n];

	if(!(region = map_lookup(vmp,
	  arch_vir2map(vmp, (vir_bytes) m->VMUM_ADDR)))) {
		return EINVAL;
	}

	if(!(region->flags & VR_DIRECT)) {
		return EINVAL;
	}

	if(map_unmap_region(vmp, region) != OK) {
		return EINVAL;
	}

	return OK;
}
