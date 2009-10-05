/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <servers/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/safecopies.h>
#include <nucleos/bitmap.h>

#include <nucleos/errno.h>
#include <nucleos/string.h>
#include <env.h>
#include <stdio.h>
#include <nucleos/fcntl.h>
#include <nucleos/signal.h>

#include <servers/vm/glo.h>
#include <servers/vm/proto.h>
#include <servers/vm/util.h>
#include <servers/vm/region.h>
#include <asm/servers/vm/memory.h>
#include <asm/pagefaults.h>

/*===========================================================================*
 *				pf_errstr	     		     	*
 *===========================================================================*/
char *pf_errstr(u32_t err)
{
	static char buf[100];

	sprintf(buf, "err 0x%lx ", err);
	if(PFERR_NOPAGE(err)) strcat(buf, "nopage ");
	if(PFERR_PROT(err)) strcat(buf, "protection ");
	if(PFERR_WRITE(err)) strcat(buf, "write");
	if(PFERR_READ(err)) strcat(buf, "read");

	return buf;
}

/*===========================================================================*
 *				do_pagefaults	     		     *
 *===========================================================================*/
void do_pagefaults(void)
{
	endpoint_t ep;
	u32_t addr, err;
	struct vmproc *vmp;
	int r, s;

	while((r=arch_get_pagefault(&ep, (vir_bytes*)&addr, &err)) == 0) {
		struct vir_region *region;
		vir_bytes offset;
		int p, wr = PFERR_WRITE(err);

#if 0
		printf("VM: pagefault: ep %d 0x%lx %s\n", 
			ep, arch_map2vir(vmp, addr), pf_errstr(err));
#endif

		if(vm_isokendpt(ep, &p) != 0)
			vm_panic("do_pagefaults: endpoint wrong", ep);

		vmp = &vmproc[p];
		vm_assert(vmp->vm_flags & VMF_INUSE);

#if 0
		map_printmap(vmp);
#endif

		/* See if address is valid at all. */
		if(!(region = map_lookup(vmp, addr))) {
			vm_assert(PFERR_NOPAGE(err));
			printf("VM: pagefault: SIGSEGV %d bad addr 0x%lx %s\n", 
				ep, arch_map2vir(vmp, addr), pf_errstr(err));
			sys_sysctl_stacktrace(vmp->vm_endpoint);
			if((s=sys_kill(vmp->vm_endpoint, SIGSEGV)) != 0)
				vm_panic("sys_kill failed", s);
			if((s=sys_vmctl(ep, VMCTL_CLEAR_PAGEFAULT, r)) != 0)
				vm_panic("do_pagefaults: sys_vmctl failed", ep);
			continue;
		}

		/* Make sure this isn't a region that isn't supposed
		 * to cause pagefaults.
		 */
		vm_assert(!(region->flags & VR_NOPF));

		/* We do not allow shared memory to cause pagefaults.
		 * These pages have to be pre-allocated.
		 */
		vm_assert(!(region->flags & VR_SHARED));

		/* If process was writing, see if it's writable. */
		if(!(region->flags & VR_WRITABLE) && wr) {
			printf("VM: pagefault: SIGSEGV %d ro map 0x%lx %s\n", 
				ep, arch_map2vir(vmp, addr), pf_errstr(err));
			sys_sysctl_stacktrace(vmp->vm_endpoint);
			if((s=sys_kill(vmp->vm_endpoint, SIGSEGV)) != 0)
				vm_panic("sys_kill failed", s);
			if((s=sys_vmctl(ep, VMCTL_CLEAR_PAGEFAULT, r)) != 0)
				vm_panic("do_pagefaults: sys_vmctl failed", ep);
			continue;
		}

		vm_assert(addr >= region->vaddr);
		offset = addr - region->vaddr;

		/* Access is allowed; handle it. */
		if((r=map_pf(vmp, region, offset, wr)) != 0) {
			printf("VM: pagefault: SIGSEGV %d pagefault not handled\n", ep);
			sys_sysctl_stacktrace(vmp->vm_endpoint);
			if((s=sys_kill(vmp->vm_endpoint, SIGSEGV)) != 0)
				vm_panic("sys_kill failed", s);
			if((s=sys_vmctl(ep, VMCTL_CLEAR_PAGEFAULT, r)) != 0)
				vm_panic("do_pagefaults: sys_vmctl failed", ep);
			continue;
		}

#if 0
		printf("VM: map_pf done; ep %d 0x%lx %s\n",
			ep, arch_map2vir(vmp, addr), pf_errstr(err));

		printf("VM: handling pagefault 0: %d addr 0x%lx %s\n",
			ep, arch_map2vir(vmp, addr), pf_errstr(err));
#endif

		/* Pagefault is handled, so now reactivate the process. */
		if((s=sys_vmctl(ep, VMCTL_CLEAR_PAGEFAULT, r)) != 0)
			vm_panic("do_pagefaults: sys_vmctl failed", ep);
	}

	return;
}

/*===========================================================================*
 *				do_memory	     		     *
 *===========================================================================*/
void do_memory(void)
{
	int r, s;
	endpoint_t who, requestor;
	vir_bytes mem;
	vir_bytes len;
	int wrflag;

	while((r=sys_vmctl_get_memreq(&who, &mem, &len, &wrflag, &requestor))
	  == 0) {
		int p, r = 0;
		struct vmproc *vmp;

		if(vm_isokendpt(who, &p) != 0)
			vm_panic("do_memory: endpoint wrong", who);
		vmp = &vmproc[p];

		r = handle_memory(vmp, mem, len, wrflag);

		if(sys_vmctl(requestor, VMCTL_MEMREQ_REPLY, r) != 0)
			vm_panic("do_memory: sys_vmctl failed", r);

#if 0
		printf("VM: handling memory request %d done 0\n",
			who);
#endif
	}
}

int handle_memory(struct vmproc *vmp, vir_bytes mem, vir_bytes len, int wrflag)
{
	struct vir_region *region;
	vir_bytes o;
	int r;

#if 0
	printf("VM: handling memory request: %d, 0x%lx-0x%lx, wr %d\n",
		vmp->vm_endpoint, mem, mem+len, wrflag);
#endif

	/* Page-align memory and length. */
	o = mem % VM_PAGE_SIZE;
	mem -= o;
	len += o;
	o = len % VM_PAGE_SIZE;
	if(o > 0) len += VM_PAGE_SIZE - o;

	if(!(region = map_lookup(vmp, mem))) {
		map_printmap(vmp);
		printf("VM: do_memory: memory doesn't exist\n");
		r = -EFAULT;
	} else if(mem + len > region->vaddr + region->length) {
		vm_assert(region->vaddr <= mem);
		vm_panic("do_memory: not contained", NO_NUM);
	} else if(!(region->flags & VR_WRITABLE) && wrflag) {
		printf("VM: do_memory: write to unwritable map\n");
		r = -EFAULT;
	} else {
		vir_bytes offset;
		vm_assert(region->vaddr <= mem);
		vm_assert(!(region->flags & VR_NOPF));
		vm_assert(!(region->vaddr % VM_PAGE_SIZE));
		offset = mem - region->vaddr;

		r = map_handle_memory(vmp, region, offset, len, wrflag);
	}

	if(r != 0) {
		printf("VM: memory range 0x%lx-0x%lx not available in %d\n",
			arch_map2vir(vmp, mem), arch_map2vir(vmp, mem+len),
			vmp->vm_endpoint);
	}
}
