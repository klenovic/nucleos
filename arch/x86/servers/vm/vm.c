/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <nucleos/bitmap.h>

#include <nucleos/mman.h>

#include <nucleos/errno.h>
#include <env.h>

#include <servers/vm/proto.h>
#include <servers/vm/vm.h>
#include <servers/vm/util.h>

#include <asm/servers/vm/memory.h>

vir_bytes arch_map2vir(struct vmproc *vmp, vir_bytes addr)
{
#if SANITYCHECKS
	vir_bytes textstart = CLICK2ABS(vmp->vm_arch.vm_seg[T].mem_phys);
#endif
	vir_bytes datastart = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	/* Could be a text address. */
	vm_assert(datastart <= addr || textstart <= addr);

	return addr - datastart;
}

char *arch_map2str(struct vmproc *vmp, vir_bytes addr)
{
	static char bufstr[100];
	vir_bytes textstart = CLICK2ABS(vmp->vm_arch.vm_seg[T].mem_phys);
	vir_bytes datastart = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	if(addr < textstart) {
		sprintf(bufstr, "<lin:0x%lx>", addr);
	} else if(addr < datastart) {
		sprintf(bufstr, "0x%lx (codeseg)", addr - textstart);
	} else {
		sprintf(bufstr, "0x%lx (dataseg)", addr - datastart);
	}

	return bufstr;
}

vir_bytes arch_addrok(struct vmproc *vmp, vir_bytes addr)
{
	vir_bytes textstart = CLICK2ABS(vmp->vm_arch.vm_seg[T].mem_phys);
	vir_bytes textend = CLICK2ABS(vmp->vm_arch.vm_seg[T].mem_phys +
		vmp->vm_arch.vm_seg[T].mem_phys);
	vir_bytes datastart = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	if(addr >= textstart && addr < textstart+textend)
		return 1;

	if(addr >= datastart && addr < VM_DATATOP)
		return 1;

	return 0;
}

vir_bytes arch_vir2map(struct vmproc *vmp, vir_bytes addr)
{
	vir_bytes bottom = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	return addr + bottom;
}

vir_bytes arch_vir2map_text(struct vmproc *vmp, vir_bytes addr)
{
	vir_bytes bottom = CLICK2ABS(vmp->vm_arch.vm_seg[T].mem_phys);

	return addr + bottom;
}
