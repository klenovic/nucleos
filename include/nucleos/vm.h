/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Prototypes and definitions for VM interface. */
#ifndef __VM_H
#define __VM_H

#include <nucleos/types.h>
#include <nucleos/endpoint.h>

_PROTOTYPE( int vm_exit, (endpoint_t ep));
_PROTOTYPE( int vm_fork, (endpoint_t ep, int slotno, int *child_ep));
_PROTOTYPE( int vm_brk, (endpoint_t ep, char *newaddr));
_PROTOTYPE( int vm_exec_newmem, (endpoint_t ep, struct exec_newmem *args,
	int args_bytes, char **ret_stack_top, int *ret_flags));
_PROTOTYPE( int vm_push_sig, (endpoint_t ep, vir_bytes *old_sp));
_PROTOTYPE( int vm_willexit, (endpoint_t ep));
_PROTOTYPE( int vm_adddma, (endpoint_t req_e, endpoint_t proc_e, 
                                phys_bytes start, phys_bytes size)      );
_PROTOTYPE( int vm_deldma, (endpoint_t req_e, endpoint_t proc_e, 
                                phys_bytes start, phys_bytes size)      );
_PROTOTYPE( int vm_getdma, (endpoint_t req_e, endpoint_t *procp,
				phys_bytes *basep, phys_bytes *sizep)   );
_PROTOTYPE( void *vm_map_phys, (endpoint_t who, void *physaddr, size_t len));
_PROTOTYPE( int vm_unmap_phys, (endpoint_t who, void *vaddr, size_t len));

_PROTOTYPE( int vm_allocmem, (phys_clicks memclicks, phys_clicks *retmembase));

#endif /* __VM_H */
