/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __ASM_X86_PAGEFAULTS_H
#define __ASM_X86_PAGEFAULTS_H

#include <asm/servers/vm/vm.h>

#define PFERR_NOPAGE(e)	(!((e) & I386_VM_PFE_P))
#define PFERR_PROT(e)	(((e) & I386_VM_PFE_P))
#define PFERR_WRITE(e)	((e) & I386_VM_PFE_W)
#define PFERR_READ(e)	(!((e) & I386_VM_PFE_W))

#endif /* __ASM_X86_PAGEFAULTS_H */

