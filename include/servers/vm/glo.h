/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VM_GLO_H
#define __SERVERS_VM_GLO_H

#include <sys/stat.h>
#include <nucleos/a.out.h>
#include <tools.h>

#include <servers/vm/vm.h>
#include <servers/vm/vmproc.h>

extern struct vmproc vmproc[NR_PROCS+1];

#if SANITYCHECKS
extern int nocheck;
u32_t data1[200];
#define CHECKADDR 0
extern long vm_sanitychecklevel;
#endif

#define VMP_SYSTEM	NR_PROCS

/* vm operation mode state and values */
extern long vm_paged;
extern phys_bytes kernel_top_bytes;

#endif /* __SERVERS_VM_GLO_H */
