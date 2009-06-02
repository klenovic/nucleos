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

#include <errno.h>
#include <env.h>

#include "glo.h"
#include "proto.h"
#include "util.h"
#include "sanitycheck.h"

PUBLIC void free_proc(struct vmproc *vmp)
{
	vmp->vm_flags &= ~VMF_HASPT;
	pt_free(&vmp->vm_pt);
	map_free_proc(vmp);
	vmp->vm_regions = NULL;
#if VMSTATS
	vmp->vm_bytecopies = 0;
#endif
}

PUBLIC void clear_proc(struct vmproc *vmp)
{
	vmp->vm_regions = NULL;
	vmp->vm_callback = NULL;	/* No pending vfs callback. */
	vmp->vm_flags = 0;		/* Clear INUSE, so slot is free. */
	vmp->vm_count = 0;
	vmp->vm_heap = NULL;
#if VMSTATS
	vmp->vm_bytecopies = 0;
#endif
}

/*===========================================================================*
 *				do_exit					     *
 *===========================================================================*/
PUBLIC int do_exit(message *msg)
{
	int proc;
	struct vmproc *vmp;

SANITYCHECK(SCL_FUNCTIONS);

	if(vm_isokendpt(msg->VME_ENDPOINT, &proc) != OK) {
		printf("VM: bogus endpoint VM_EXIT %d\n", msg->VME_ENDPOINT);
		return EINVAL;
	}
	vmp = &vmproc[proc];
	if(!(vmp->vm_flags & VMF_EXITING)) {
		printf("VM: unannounced VM_EXIT %d\n", msg->VME_ENDPOINT);
		return EINVAL;
	}

	if(vmp->vm_flags & VMF_HAS_DMA) {
		release_dma(vmp);
	} else if(vmp->vm_flags & VMF_HASPT) {
		/* Free pagetable and pages allocated by pt code. */
SANITYCHECK(SCL_DETAIL);
		free_proc(vmp);
SANITYCHECK(SCL_DETAIL);
	} else {
		/* Free the data and stack segments. */
		FREE_MEM(vmp->vm_arch.vm_seg[D].mem_phys,
			vmp->vm_arch.vm_seg[S].mem_vir +
			vmp->vm_arch.vm_seg[S].mem_len -
			vmp->vm_arch.vm_seg[D].mem_vir);

		if (find_share(vmp, vmp->vm_ino, vmp->vm_dev, vmp->vm_ctime) == NULL) {
			/* No other process shares the text segment,
			 * so free it.
			 */
			FREE_MEM(vmp->vm_arch.vm_seg[T].mem_phys,
			vmp->vm_arch.vm_seg[T].mem_len);
		}
	}
SANITYCHECK(SCL_DETAIL);

	/* Reset process slot fields. */
	clear_proc(vmp);

SANITYCHECK(SCL_FUNCTIONS);
	return OK;
}

/*===========================================================================*
 *				do_willexit				     *
 *===========================================================================*/
PUBLIC int do_willexit(message *msg)
{
	int proc;
	struct vmproc *vmp;

	if(vm_isokendpt(msg->VMWE_ENDPOINT, &proc) != OK) {
		printf("VM: bogus endpoint VM_EXITING %d\n",
			msg->VMWE_ENDPOINT);
		return EINVAL;
	}
	vmp = &vmproc[proc];

	vmp->vm_flags |= VMF_EXITING;

	return OK;
}

PUBLIC void _exit(int code)
{
  sys_exit(SELF);
}

PUBLIC void __exit(int code)
{
  sys_exit(SELF);
}
