/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_KERNEL_UACCESS_32_H
#define __ASM_X86_KERNEL_UACCESS_32_H

/*
 * User space memory access functions
 */
#include <nucleos/errno.h>
#include <nucleos/type.h>
#include <kernel/proto.h>
#include <kernel/glo.h>
#include <kernel/vm.h>

/**
 * Copy data to user space from kernel space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 */
static inline unsigned long copy_to_user(void __user *to, const void *from, unsigned long n)
{
	phys_bytes linaddr;
	phys_bytes pfaddr;

	/* @nucleos: Do better address checking! */
	if (!to || !from)
		return -EINVAL;

	if (!(linaddr = umap_local(proc_ptr, D, (vir_bytes)to, n))) {
		return -EFAULT;
	}

	/* We must use this version of phys_copy (wrapper) because a pagefault
	 * exception may occur during copy and we want to handle it.
	 */
	pfaddr = PHYS_COPY_CATCH(vir2phys(from), linaddr, n);

	if (!pfaddr)
		return 0;

	if (pfaddr < linaddr || n < (pfaddr - linaddr))
		return n;

	return (n - (pfaddr - linaddr));
}

/**
 * Copy data from user space to kernel space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 */
static inline unsigned long copy_from_user(void *to, const void __user *from, unsigned long n)
{
	phys_bytes linaddr;
	phys_bytes pfaddr;

	/* @nucleos: Do better address checking! */
	if (!to || !from)
		return -EINVAL;

	if (!(linaddr = umap_local(proc_ptr, D, (vir_bytes)from, n))) {
		return -EFAULT;
	}

	/* We must use this version of phys_copy (wrapper) because a pagefault
	 * exception may occur during copy and we want to handle it.
	 */
	pfaddr = PHYS_COPY_CATCH(linaddr, vir2phys(to), n);

	if (!pfaddr)
		return 0;

	if (pfaddr < linaddr || n < (pfaddr - linaddr))
		return n;

	return (n - (pfaddr - linaddr));
}

#endif /*__ASM_X86_KERNEL_UACCESS_32_H */
