/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/config.h>
#include <sys/stat.h>
#include <a.out.h>
#include <tools.h>

#include "vm.h"
#include "vmproc.h"

#if _MAIN
#undef EXTERN
#define EXTERN
#endif

EXTERN struct vmproc vmproc[_NR_PROCS+1];

#if SANITYCHECKS
EXTERN int nocheck;
u32_t data1[200];
#define CHECKADDR 0
EXTERN long vm_sanitychecklevel;
#endif

#define VMP_SYSTEM	_NR_PROCS

/* vm operation mode state and values */
EXTERN long vm_paged;
EXTERN phys_bytes kernel_top_bytes;
