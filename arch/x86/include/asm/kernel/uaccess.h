/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * User space memory access functions
 */
#ifndef __ASM_X86_KERNEL_UACCESS_H
#define __ASM_X86_KERNEL_UACCESS_H

#ifdef CONFIG_X86_32
#  include "uaccess_32.h"
#else
#  include "uaccess_64.h"
#endif

#endif /* __ASM_X86_KERNEL_UACCESS_H */
