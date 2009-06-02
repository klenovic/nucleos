/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#ifndef __X86_ASM_BOOTPARAM_H
#define __X86_ASM_BOOTPARAM_H

#include <asm/types.h>

struct boot_param {
	__uint32_t initrd_base; /* physical address of initial ramdisk */
	__uint32_t initrd_size; /* size of initial ramdisk in bytes */
};

#endif /* !__X86_ASM_BOOTPARAM_H */
