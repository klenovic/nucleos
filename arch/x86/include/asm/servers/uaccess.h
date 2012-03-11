/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_SERVERS_UACCESS_H
#define __ASM_X86_SERVERS_UACCESS_H
/*
 * User space memory access functions
 */
#ifdef CONFIG_X86_32
#  include "uaccess_32.h"
#else
#  include "uaccess_64.h"
#endif

#endif /*__ASM_X86_SERVERS_UACCESS_H */
