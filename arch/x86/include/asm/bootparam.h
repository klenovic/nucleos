/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @file arch/x86/include/asm/bootparam.h
 * @brief Boot parameters
 * @details Interfaces to handle the x86 boot parameters.
 */
#ifndef __ASM_X86_BOOTPARAM_H
#define __ASM_X86_BOOTPARAM_H

#include <nucleos/types.h>

struct boot_param {
	__u32 initrd_base; /* physical address of initial ramdisk */
	__u32 initrd_size; /* size of initial ramdisk in bytes */
};

#endif /* !__ASM_X86_BOOTPARAM_H */
