/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Prototypes and definitions for VM interface. */
#ifndef __NUCLEOS_VM_H
#define __NUCLEOS_VM_H

#include <nucleos/types.h>
#include <nucleos/type.h>
#include <nucleos/endpoint.h>

int vm_exit(endpoint_t ep);
int vm_fork(endpoint_t ep, int slotno, endpoint_t *child_ep);
int vm_brk(endpoint_t ep, char *newaddr);
int vm_exec_newmem(endpoint_t ep, struct exec_newmem *args, int args_bytes, char **ret_stack_top,
		   int *ret_flags);
int vm_push_sig(endpoint_t ep, vir_bytes *old_sp);
int vm_willexit(endpoint_t ep);
int vm_adddma(endpoint_t req_e, endpoint_t proc_e, phys_bytes start, phys_bytes size);
int vm_deldma(endpoint_t req_e, endpoint_t proc_e, phys_bytes start, phys_bytes size);
int vm_getdma(endpoint_t req_e, endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep);
void *vm_map_phys(endpoint_t who, void *physaddr, size_t len);
int vm_unmap_phys(endpoint_t who, void *vaddr, size_t len);

int vm_allocmem(phys_clicks memclicks, phys_clicks *retmembase);
int vm_notify_sig(endpoint_t ep, endpoint_t ipc_ep);
int vm_ctl(int what, int param);
int vm_set_priv(int procnr, void *buf);
int vm_query_exit(int *endpt);

#endif /* __NUCLEOS_VM_H */
