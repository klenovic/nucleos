/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/unistd.h>
#include <nucleos/mman.h>
#include <nucleos/errno.h>
#include <asm/syscall.h>
#include <stdarg.h>

int __munmap_text(void *addr, size_t len)
{
	return INLINE_SYSCALL(munmap_text, 2, addr, len);
}

/* munamp_text just a weak definition since PM/VM may override it */
int munmap_text(void *addr, size_t len) __weak __alias("__munmap_text");
