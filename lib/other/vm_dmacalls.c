/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/lib.h>
#include <nucleos/vm.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int vm_adddma(endpoint_t req_proc_e, endpoint_t proc_e, phys_bytes start, phys_bytes size)
{
  kipc_msg_t m;

  m.VMAD_REQ= req_proc_e;
  m.VMAD_EP= proc_e;
  m.VMAD_START= start;
  m.VMAD_SIZE= size;

  return ktaskcall(VM_PROC_NR, VM_ADDDMA, &m);
}

int vm_deldma(endpoint_t req_proc_e, endpoint_t proc_e, phys_bytes start, phys_bytes size)
{
  kipc_msg_t m;

  m.VMDD_REQ= proc_e;
  m.VMDD_EP= proc_e;
  m.VMDD_START= start;
  m.VMDD_SIZE= size;

  return ktaskcall(VM_PROC_NR, VM_DELDMA, &m);
}

int vm_getdma(endpoint_t req_proc_e, endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep)
{
  int r;
  kipc_msg_t m;

  m.VMGD_REQ = req_proc_e;

  r= ktaskcall(VM_PROC_NR, VM_GETDMA, &m);
  if (r == 0)
  {
	*procp= m.VMGD_PROCP;
	*basep= m.VMGD_BASEP;
	*sizep= m.VMGD_SIZEP;
  }
  return r;
}

