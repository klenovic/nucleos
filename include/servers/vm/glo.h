/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VM_GLO_H
#define __SERVERS_VM_GLO_H

#include <nucleos/stat.h>
#include <nucleos/a.out.h>
#include <tools.h>

#include <servers/vm/vm.h>
#include <servers/vm/vmproc.h>

#define VMP_SYSTEM	NR_PROCS
#define VMP_EXECTMP	NR_PROCS + 1
#define VMP_NR		NR_PROCS + 2

extern struct vmproc vmproc[];

#if SANITYCHECKS
extern int nocheck;
extern int incheck;
extern long vm_sanitychecklevel;
#endif

/* vm operation mode state and values */
extern long vm_paged;
extern int meminit_done;

#endif /* __SERVERS_VM_GLO_H */
