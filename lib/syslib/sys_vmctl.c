/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

int sys_vmctl(endpoint_t who, int param, u32_t value)
{
  kipc_msg_t m;
  int r;

  m.SVMCTL_WHO = who;
  m.SVMCTL_PARAM = param;
  m.SVMCTL_VALUE = value;
  r = ktaskcall(SYSTASK, SYS_VMCTL, &m);
  return(r);
}

int sys_vmctl_get_pagefault_i386(endpoint_t *who, u32_t *cr2, u32_t *err)
{
  kipc_msg_t m;
  int r;

  m.SVMCTL_WHO = ENDPT_SELF;
  m.SVMCTL_PARAM = VMCTL_GET_PAGEFAULT;
  r = ktaskcall(SYSTASK, SYS_VMCTL, &m);
  if(r == 0) {
	*who = m.SVMCTL_PF_WHO;
	*cr2 = m.SVMCTL_PF_I386_CR2;
	*err = m.SVMCTL_PF_I386_ERR;
  }
  return(r);
}

int sys_vmctl_get_cr3_i386(endpoint_t who, u32_t *cr3)
{
  kipc_msg_t m;
  int r;

  m.SVMCTL_WHO = who;
  m.SVMCTL_PARAM = VMCTL_I386_GETCR3;
  r = ktaskcall(SYSTASK, SYS_VMCTL, &m);
  if(r == 0) {
	*cr3 = m.SVMCTL_VALUE;
  }
  return(r);
}

int sys_vmctl_get_memreq(endpoint_t *who, vir_bytes *mem,
	vir_bytes *len, int *wrflag, endpoint_t *requestor)
{
  kipc_msg_t m;
  int r;

  m.SVMCTL_WHO = ENDPT_SELF;
  m.SVMCTL_PARAM = VMCTL_MEMREQ_GET;
  r = ktaskcall(SYSTASK, SYS_VMCTL, &m);
  if(r == 0) {
	*who = m.SVMCTL_MRG_EP;
	*mem = (vir_bytes) m.SVMCTL_MRG_ADDR;
	*len = m.SVMCTL_MRG_LEN;
	*wrflag = m.SVMCTL_MRG_WRITE;
	*requestor = (endpoint_t) m.SVMCTL_MRG_REQUESTOR;
  }
  return r;
}

int sys_vmctl_enable_paging(struct mem_map *map)
{
	kipc_msg_t m;
	m.SVMCTL_WHO = ENDPT_SELF;
	m.SVMCTL_PARAM = VMCTL_ENABLE_PAGING;
	m.SVMCTL_VALUE = (int) map;
	return ktaskcall(SYSTASK, SYS_VMCTL, &m);
}

int sys_vmctl_get_mapping(int index,
	phys_bytes *addr, phys_bytes *len, int *flags)
{
	int r;
	kipc_msg_t m;

	m.SVMCTL_WHO = ENDPT_SELF;
	m.SVMCTL_PARAM = VMCTL_KERN_PHYSMAP;
	m.SVMCTL_VALUE = (int) index;

	r = ktaskcall(SYSTASK, SYS_VMCTL, &m);

	if(r != 0)
		return r;

	*addr = m.SVMCTL_MAP_PHYS_ADDR;
	*len = m.SVMCTL_MAP_PHYS_LEN;
	*flags = m.SVMCTL_MAP_FLAGS;

	return 0;
}

int sys_vmctl_reply_mapping(int index, vir_bytes addr)
{
	int r;
	kipc_msg_t m;

	m.SVMCTL_WHO = ENDPT_SELF;
	m.SVMCTL_PARAM = VMCTL_KERN_MAP_REPLY;
	m.SVMCTL_VALUE = index;
	m.SVMCTL_MAP_VIR_ADDR = (char *) addr;

	return ktaskcall(SYSTASK, SYS_VMCTL, &m);
}
