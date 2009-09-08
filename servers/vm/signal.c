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
#include <asm/sigcontext.h>

#include <nucleos/errno.h>
#include <env.h>

#include <servers/vm/glo.h>
#include <servers/vm/vm.h>
#include <servers/vm/proto.h>
#include <servers/vm/util.h>

#define DATA_CHANGED       1    /* flag value when data segment size changed */
#define STACK_CHANGED      2    /* flag value when stack size changed */

/*===========================================================================*
 *				do_push_sig				     *
 *===========================================================================*/
int do_push_sig(message *msg)
{
	int r, n;
	endpoint_t ep;
	vir_bytes sp;
	struct vmproc *vmp;

	ep = msg->VMPS_ENDPOINT;

	if((r=vm_isokendpt(ep, &n)) != 0) {
		printf("VM: bogus endpoint %d from %d\n", ep, msg->m_source);
		return r;
	}
	vmp = &vmproc[n];

        if ((r=get_stack_ptr(ep, &sp)) != 0)
                vm_panic("couldn't get new stack pointer (for sig)",r);
	
	/* Save old SP for caller */
	msg->VMPS_OLD_SP = (char *) sp;
   
        /* Make room for the sigcontext and sigframe struct. */
        sp -= sizeof(struct sigcontext)
                                 + 3 * sizeof(char *) + 2 * sizeof(int);

        if ((r=adjust(vmp, vmp->vm_arch.vm_seg[D].mem_len, sp)) != 0) {
		printf("VM: do_push_sig: adjust() failed: %d\n", r);
		return r;
	}

	return 0;
}

