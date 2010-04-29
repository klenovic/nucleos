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
#include <asm/servers/vm/memory.h>

extern int __phys_strnlen(const char *s, size_t maxlen);

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

	vm_set_cr3(proc_ptr);

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

	vm_set_cr3(proc_ptr);

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

/**
 * Get the size of a string specified by linear address.
 * @proc  process address
 * @s  The string to measure (linear address).
 * @maxlen  The maximum valid length
 *
 * Get the size of a NUL-terminated string.
 *
 * Returns the size of the string _including_ the terminating NULL.
 * On kernel exception, returns 0.
 * If the string is too long, returns a value greater than @maxlen.
 */
static inline long strnlen_user(struct proc *proc, const char __user *s, size_t maxlen)
{
	unsigned long len = (VM_STACKTOP - (unsigned long)s);
	phys_bytes linaddr;

	/* We must not cross the top of stack during copy */
	len = (len < maxlen) ? len : maxlen;

	if (!(linaddr = umap_local(proc, D, (vir_bytes)s, len)))
		return 0;

	vm_set_cr3(proc);

	/* might fault */
	catch_pagefaults++;
	/* The __phys_strnlen returns the size _including_ the '\0'.
	 * In case of exception in kernel returns 0.
	 */
	len = __phys_strnlen((const char __user*)linaddr, len);
	catch_pagefaults--;

	return len;
}

#endif /*__ASM_X86_KERNEL_UACCESS_32_H */
