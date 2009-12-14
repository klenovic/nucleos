/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_UACCESS_H
#define __ASM_X86_UACCESS_H
/*
 * User space memory access functions
 */

#ifdef __KERNEL__
#  include <asm/kernel/uaccess.h>
#elif __UKERNEL__ /* ukernel {servers, drivers} */
#  include <asm/servers/uaccess.h>
#endif /*__UKERNEL__ */

#endif /* __ASM_X86_UACCESS_H */
