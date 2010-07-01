/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_SERVERS_UACCESS_32_H
#define __ASM_X86_SERVERS_UACCESS_32_H

/*
 * User space memory access functions
 */
#include <nucleos/errno.h>
#include <nucleos/type.h>
#include <nucleos/proto.h>

/*
 * Copy data to user space from ukernel (server/driver) space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 */
static inline int uk_copy_to_user(void __user *to, const void *from, unsigned long n)
{
}

/*
 * Copy data from user space to ukernel (server/driver) space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 */
static inline int uk_copy_from_user(void *to, const void __user *from, unsigned long n)
{
}

#endif /* __ASM_X86_SERVERS_UACCESS_32_H */
