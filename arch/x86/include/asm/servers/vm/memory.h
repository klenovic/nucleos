/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_SERVERS_VM_MEMORY_H
#define __ASM_X86_SERVERS_VM_MEMORY_H

#include <asm/servers/vm/vm.h>

/* As visible from the user space process, where is the top of the
 * stack (first non-stack byte), when in paged mode?
 */
#define VM_STACKTOP     0x80000000

/* And what is the highest addressable piece of memory, when in paged
 * mode? Some data for kernel and stack are subtracted from this, the
 * final results stored in bytes in arch.vm_data_top.
 */
#define VM_DATATOP	0xFFFFF000

#define SLAB_PAGESIZE	I386_PAGE_SIZE
#define VM_PAGE_SIZE	I386_PAGE_SIZE

/* Where do processes start in linear (i.e. page table) memory? */
#define VM_PROCSTART	(I386_BIG_PAGE_SIZE*100)

#define CLICKSPERPAGE	(I386_PAGE_SIZE/CLICK_SIZE)

/* Where is the kernel? */
#define KERNEL_TEXT		CLICK2ABS(vmproc[VMP_SYSTEM].vm_arch.vm_seg[T].mem_phys)
#define KERNEL_TEXT_LEN		CLICK2ABS(vmproc[VMP_SYSTEM].vm_arch.vm_seg[T].mem_len)
#define KERNEL_DATA		CLICK2ABS(vmproc[VMP_SYSTEM].vm_arch.vm_seg[D].mem_phys)
#define KERNEL_DATA_LEN		CLICK2ABS(vmproc[VMP_SYSTEM].vm_arch.vm_seg[D].mem_len \
					  + vmproc[VMP_SYSTEM].vm_arch.vm_seg[S].mem_len)
#endif /* __ASM_X86_SERVERS_VM_MEMORY_H */
