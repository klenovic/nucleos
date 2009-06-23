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

#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <server/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/ipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>

#include <sys/mman.h>

#include <errno.h>
#include <env.h>

#include "../proto.h"
#include "../vm.h"
#include "../util.h"

#include <asm/server/vm/memory.h>

#define PAGE_SIZE	4096
#define PAGE_DIR_SIZE	(1024*PAGE_SIZE)	
#define PAGE_TABLE_COVER (1024*PAGE_SIZE)
/*=========================================================================*
 *				arch_init_vm				   *
 *=========================================================================*/
PUBLIC void arch_init_vm(mem_chunks)
struct memory mem_chunks[NR_MEMS];
{
	phys_bytes high, bytes;
	phys_clicks clicks, base_click;
	unsigned pages;
	int i, r;

	/* Compute the highest memory location */
	high= 0;
	for (i= 0; i<NR_MEMS; i++)
	{
		if (mem_chunks[i].size == 0)
			continue;
		if (mem_chunks[i].base + mem_chunks[i].size > high)
			high= mem_chunks[i].base + mem_chunks[i].size;
	}

	high <<= CLICK_SHIFT;
#if VERBOSE_VM
	printf("do_x86_vm: found high 0x%x\n", high);
#endif
	
	/* Rounding up */
	high= (high-1+PAGE_DIR_SIZE) & ~(PAGE_DIR_SIZE-1);

	/* The number of pages we need is one for the page directory, enough
	 * page tables to cover the memory, and one page for alignement.
	 */
	pages= 1 + (high + PAGE_TABLE_COVER-1)/PAGE_TABLE_COVER + 1;
	bytes= pages*PAGE_SIZE;
	clicks= (bytes + CLICK_SIZE-1) >> CLICK_SHIFT;

#if VERBOSE_VM
	printf("do_x86_vm: need %d pages\n", pages);
	printf("do_x86_vm: need %d bytes\n", bytes);
	printf("do_x86_vm: need %d clicks\n", clicks);
#endif

	for (i= 0; i<NR_MEMS; i++)
	{
		if (mem_chunks[i].size <= clicks)
			continue;
		break;
	}
	if (i >= NR_MEMS)
		panic("VM", "not enough memory for VM page tables?", NO_NUM);
	base_click= mem_chunks[i].base;
	mem_chunks[i].base += clicks;
	mem_chunks[i].size -= clicks;

#if VERBOSE_VM
	printf("do_x86_vm: using 0x%x clicks @ 0x%x\n", clicks, base_click);
#endif
	r= sys_vm_setbuf(base_click << CLICK_SHIFT, clicks << CLICK_SHIFT,
		high);
	if (r != 0)
		printf("do_x86_vm: sys_vm_setbuf failed: %d\n", r);

}

/*===========================================================================*
 *				arch_map2vir				     *
 *===========================================================================*/
PUBLIC vir_bytes arch_map2vir(struct vmproc *vmp, vir_bytes addr)
{
	vir_bytes bottom = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	vm_assert(bottom <= addr);

	return addr - bottom;
}

/*===========================================================================*
 *				arch_vir2map				     *
 *===========================================================================*/
PUBLIC vir_bytes arch_vir2map(struct vmproc *vmp, vir_bytes addr)
{
	vir_bytes bottom = CLICK2ABS(vmp->vm_arch.vm_seg[D].mem_phys);

	return addr + bottom;
}
