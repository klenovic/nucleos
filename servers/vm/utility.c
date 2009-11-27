/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* This file contains some utility routines for VM.  */

#include <asm/bootparam.h>
#include <nucleos/kernel.h>
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <servers/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/type.h>
#include <nucleos/bitmap.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <env.h>

#include <servers/vm/proto.h>
#include <servers/vm/glo.h>
#include <servers/vm/util.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <kernel/proc.h>
#include <asm/kernel/const.h>
#include <asm/servers/vm/memory.h>

/*===========================================================================*
 *                              get_mem_map                                  *
 *===========================================================================*/
int get_mem_map(proc_nr, mem_map)
int proc_nr;                                    /* process to get map of */
struct mem_map *mem_map;                        /* put memory map here */
{
	struct proc p;
	int s;

	if ((s=sys_getproc(&p, proc_nr)) != 0)
		return(s);

	memcpy(mem_map, p.p_memmap, sizeof(p.p_memmap));
	return 0;
}

/*===========================================================================*
 *                              get_mem_chunks                               *
 *===========================================================================*/
void get_mem_chunks(mem_chunks)
struct memory *mem_chunks;                      /* store mem chunks here */ 
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
        memp = &mem_chunks[i];          /* next mem chunk is stored here */
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

/*===========================================================================*
 *                              reserve_proc_mem                             *
 *===========================================================================*/
void reserve_proc_mem(mem_chunks, map_ptr)
struct memory *mem_chunks;                      /* store mem chunks here */
struct mem_map *map_ptr;                        /* memory to remove */
{
/* Remove server memory from the free memory list. The boot monitor
 * promises to put processes at the start of memory chunks. The 
 * tasks all use same base address, so only the first task changes
 * the memory lists. The servers and init have their own memory
 * spaces and their memory will be removed from the list.
 */
  struct memory *memp;
  for (memp = mem_chunks; memp < &mem_chunks[NR_MEMS]; memp++) {
        if (memp->base == map_ptr[T].mem_phys) {
                memp->base += map_ptr[T].mem_len + map_ptr[S].mem_vir;
                memp->size -= map_ptr[T].mem_len + map_ptr[S].mem_vir;
                break;
        }
  }
  if (memp >= &mem_chunks[NR_MEMS])
  {
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
	int i;
	int s;
	int found = 0;
	struct boot_param bootparam;
	phys_bytes initrd_base_clicks;
	phys_bytes initrd_size_clicks;

	/* Don't enter if the ramdisk has been found already */
	if (found)
		return 0;

	/* The initrd is put right after boot image */
	if ((s = sys_getbootparam(&bootparam)) != 0) {
		panic("VM","Couldn't get boot parameters!",s);
	}

	initrd_base_clicks = (bootparam.initrd_base >> CLICK_SHIFT);

	/* Find initial ramdisk */
	for (i=0; i<NR_MEMS; i++) {
		if (mem_chunks[i].base == initrd_base_clicks) {
			found = 1;
			break;
		}
	}

	if (!found) {
		printf("VM: Couldn't find initial ramdisk at 0x%x\n", initrd_base_clicks);
		return -ENXIO;
	}

	/* Round up the reserved memory */
	initrd_size_clicks = ((bootparam.initrd_size + CLICK_SIZE - 1) >> CLICK_SHIFT);

	mem_chunks[i].base += initrd_size_clicks;
	mem_chunks[i].size -= initrd_size_clicks;

	return 0;
}
#endif

/*===========================================================================*
 *                              vm_isokendpt                           	     *
 *===========================================================================*/
int vm_isokendpt(endpoint_t endpoint, int *proc)
{
        *proc = _ENDPOINT_P(endpoint);
        if(*proc < 0 || *proc >= NR_PROCS)
        	vm_panic("crazy slot number", *proc); 
        if(*proc >= 0 && endpoint != vmproc[*proc].vm_endpoint) {
                return -EDEADSRCDST;
        }
        if(*proc >= 0 && !(vmproc[*proc].vm_flags & VMF_INUSE)) {
                return -EDEADSRCDST;
        }
        return 0;
}


struct proc mytmpproc;

/*===========================================================================*
 *                              get_stack_ptr                                *
 *===========================================================================*/
int get_stack_ptr(proc_nr_e, sp)
int proc_nr_e;                                  /* process to get sp of */   
vir_bytes *sp;                                  /* put stack pointer here */
{
  int s; 
  
  if ((s=sys_getproc(&mytmpproc, proc_nr_e)) != 0)     
        return(s);
  *sp = mytmpproc.p_reg.sp;
  return 0;
}       

/*===========================================================================*
 *                              brk                                          *
 *===========================================================================*/
extern void *__curbrk;

/* Our brk() must redefine brk(). */
int brk(brk_addr)
void *brk_addr;
{
        int r;
	struct vmproc *vmm = &vmproc[VM_PROC_NR];

/* VM wants to call brk() itself. */
        if((r=real_brk(vmm, (vir_bytes) brk_addr)) != 0)
		vm_panic("VM: brk() on myself failed\n", NO_NUM);
        __curbrk = brk_addr;
        return 0;
}

/*===========================================================================*
 *                              do_ctl                                        *
 *===========================================================================*/
int do_ctl(message *m)
{
	int pages, nodes;
	int pr;
	struct vmproc *vmp;

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

