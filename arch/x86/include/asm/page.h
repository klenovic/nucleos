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
 * @file arch/x86/include/asm/page.h
 * @brief
 * @details
 */
#ifndef __ASM_X86_PAGE_H
#define __ASM_X86_PAGE_H

#include <kernel/const.h>

#define PAGE_SHIFT  12
#define PAGE_SIZE   (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))

#ifdef CONFIG_X86_32
#include <asm/page_32.h>
#else
// maybe 64-bit one day :)
#endif 

#endif /* __ASM_X86_PAGE_32_H */
