/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* This file contains some utility routines for VM.  */
#include <nucleos/sysutil.h>
#include <nucleos/kernel.h>
#include <nucleos/endpoint.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/type.h>
#include <nucleos/bitmap.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <servers/vm/proto.h>
#include <servers/vm/glo.h>
#include <servers/vm/util.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <kernel/proc.h>
#include <asm/bootparam.h>
#include <asm/kernel/const.h>
#include <asm/servers/vm/memory.h>

/**
 * Get proccess memory map
 * @param proc_nr  process to get map of
 * @param mem_map  put memory map here
 */
int get_mem_map(int proc_nr, struct mem_map *mem_map)
{
	struct proc p;
	int s;

	if ((s=sys_getproc(&p, proc_nr)) != 0)
		return(s);

	memcpy(mem_map, p.p_memmap, sizeof(p.p_memmap));
	return 0;
}

/**
 * Get memory chunks
 * @param mem_chunks  store mem chunks here
 */
void get_mem_chunks(struct memory *mem_chunks)
{
/* Initialize the free memory list from the 'memory' boot variable.  Translate
 * the byte offsets and sizes in this list to clicks, properly truncated.
 */
	phys_bytes base, size, limit;
	int i;
	struct memory *memp;

	/* Obtain and parse memory from system environment. */
	if(env_memory_parse(mem_chunks, NR_MEMS) != 0) 
		vm_panic("couldn't obtain memory chunks", NO_NUM);

	/* Round physical memory to clicks. Round start up, round end down. */
	for (i = 0; i < NR_MEMS; i++) {
		memp = &mem_chunks[i];	/* next mem chunk is stored here */
		base = mem_chunks[i].base;
		size = mem_chunks[i].size;
		limit = base + size;
		base = (base + CLICK_SIZE-1) & ~(long)(CLICK_SIZE-1);
		limit &= ~(long)(CLICK_SIZE-1);
		if (limit <= base) {
			memp->base = memp->size = 0;
		} else {
			memp->base = base >> CLICK_SHIFT;
			memp->size = (limit - base) >> CLICK_SHIFT;
		}
	}
}

/**
 * Reserve memory.
 * @param mem_chunks  store mem chunks here
 * @param map_ptr  memory to remove
 */
void reserve_proc_mem(struct memory *mem_chunks, struct mem_map *map_ptr)
{
/* Remove server memory from the free memory list. The boot monitor
 * promises to put processes at the start of memory chunks. The
 * tasks all use same base address, so only the first task changes
 * the memory lists. The servers and init have their own memory
 * spaces and their memory will be removed from the list.
 */
	struct memory *memp;
	u64 proc_phys_start = map_ptr[T].mem_phys;
	u64 proc_phys_end = map_ptr[S].mem_phys;
	u64 proc_phys_size = proc_phys_end - proc_phys_start;

	for (memp = mem_chunks; memp < &mem_chunks[NR_MEMS]; memp++) {
		if (memp->base == proc_phys_start) {
			memp->base += proc_phys_size;
			memp->size -= proc_phys_size;

			break;
		}
	}

	if (memp >= &mem_chunks[NR_MEMS]) {
		vm_panic("reserve_proc_mem: can't find map in mem_chunks ",
		map_ptr[T].mem_phys);
	}
}

#ifndef CONFIG_BUILTIN_INITRD
/* @brief Remove initial ramdisk from the free memory list.
 * @param mem_chunks  list of memory chunks
 * @return 0 on success or -ENXIO on failure
 * @details The initial ramdisk is copied right after the boot image by
 *          boot monitor. It is very important to reserve initrd memory
 *          right after the boot image was reserved.
 */
int reserve_initrd_mem(struct memory *mem_chunks)
{
	int s;
	static int found = 0;
	struct boot_params bootparam;
	phys_bytes initrd_base_clicks;
	phys_bytes initrd_size_clicks;
	struct mem_map initrd_mm[3];

	/* Don't enter if the ramdisk has been found already */
	if (found)
		return 0;

	/* The initrd is put right after boot image */
	if ((s = sys_getbootparam(&bootparam)) != 0) {
		panic("VM","Couldn't get boot parameters!",s);
	}

	initrd_base_clicks = (bootparam.hdr.ramdisk_image >> CLICK_SHIFT);

	/* Round up the reserved memory */
	initrd_size_clicks = ((bootparam.hdr.ramdisk_size + CLICK_SIZE - 1) >> CLICK_SHIFT);

	/* Fake the initrd as process with text_base = data_base,
	 * stack_base = text_base + text_size and stack_size = 0
	 */
	initrd_mm[T].mem_phys = initrd_base_clicks;
	initrd_mm[D].mem_phys = initrd_mm[T].mem_phys;
	initrd_mm[S].mem_phys = initrd_mm[T].mem_phys + initrd_size_clicks;

	reserve_proc_mem(mem_chunks, initrd_mm);

	found = 1;

	return 0;
}
#endif

int vm_isokendpt(endpoint_t endpoint, int *proc)
{
	*proc = _ENDPOINT_P(endpoint);
	if(*proc < 0 || *proc >= NR_PROCS)
		vm_panic("crazy slot number", *proc);

	if(*proc >= 0 && endpoint != vmproc[*proc].vm_endpoint)
                return -EDEADSRCDST;

	if(*proc >= 0 && !(vmproc[*proc].vm_flags & VMF_INUSE))
		return -EDEADSRCDST;

	return 0;
}


struct proc mytmpproc;

/**
 * Get stack pointer
 * @param proc_nr_e  process to get sp of
 * @param sp  put stack pointer here
 */
int get_stack_ptr(int proc_nr_e, vir_bytes *sp)
{
	int s;

	if ((s=sys_getproc(&mytmpproc, proc_nr_e)) != 0)
		return(s);

	*sp = mytmpproc.p_reg.sp;

	return 0;
}

extern void *__curbrk;

/* Our brk() must redefine brk(). */
int brk(void *brk_addr)
{
	int r;
	struct vmproc *vmm = &vmproc[VM_PROC_NR];

	/* VM wants to call brk() itself. */
	if((r=real_brk(vmm, (vir_bytes) brk_addr)) != 0)
		vm_panic("VM: brk() on myself failed\n", NO_NUM);

	__curbrk = brk_addr;

	return 0;
}

int do_ctl(kipc_msg_t *m)
{
	int pr;

	switch(m->VCTL_WHAT) {
		case VCTLP_STATS_MEM:
			printmemstats();
			break;
		case VCTLP_STATS_EP:
			if(vm_isokendpt(m->VCTL_PARAM, &pr) != 0)
				return -EINVAL;
			printregionstats(&vmproc[pr]);
			break;
		default:
			return -EINVAL;
	}

	return 0;
}
