/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>
#include <asm/servers/vm/memory.h>

/**
 * Copy a string
 * @who_dst  destination endpoint
 * @dest  output buffer with the string
 * @who_src  source endpoint
 * @src  source string
 * @n  max. n bytes to be copied
 *
 * @return return zero on success; negative otherwise
 * @note If the length of src is less than n, sys_strncpy()
 *       pads the remainder of dest with null bytes.
 * @warning If there is no null byte among the first n bytes of src,
 *          the string placed in dest will not be null terminated.
 */
long sys_strncpy(endpoint_t who_dst, char *dest, endpoint_t who_src,
		char __user *src, const int n)
{
	int ret;
	int len = (VM_STACKTOP - (unsigned long)src);

	if (!dest || n <= 0)
		return -EINVAL;

	len = (len < n) ? len : n;
	len = (len <= 0) ? n : len;

	/* Get the length of the string.
	 * The sys_strnlen returns the length _including_ the '\0' character.
	 * In case of exception the zero is returned.
	 */
	len = sys_strnlen(who_src, src, len);

	/* Check whether an exception occured */
	if (!len || len < 0)
		return -EFAULT;

	/* if the source string is too long then copy just n bytes */
	if (len > n)
		len = n;

	memset(dest, 0, n);

	/* Copy from user space. */
	ret = sys_datacopy(who_src, (vir_bytes)src, who_dst,
			   (vir_bytes)dest, (phys_bytes)len);

	if (ret)
		return -EFAULT;

	return 0;
}
