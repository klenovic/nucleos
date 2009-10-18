/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* linux */
/**
 * @file arch/x86/include/asm/linkage_32.h
 * @brief linkage
 */
#ifndef __ASM_X86_LINKAGE_H
#define __ASM_X86_LINKAGE_H

#include <nucleos/stringify.h>

#undef notrace
#define notrace __attribute__((no_instrument_function))

#ifdef CONFIG_X86_32

#define __ALIGN		.p2align 2, 0x90
#define __ALIGN_STR	__stringify(__ALIGN)

/* @brief Entry for 16-bit code */
#define ENTRY16(name)		\
	.globl name;		\
	.p2align 1, 0x90;	\
	name:

#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))
/*
 * For other 32-bit functions implemented in assembly that use
 * regparm input parameters:
 */
#define asmregparm __attribute__((regparm(3)))

/*
 * Make sure the compiler doesn't do anything stupid with the
 * arguments on the stack - they are owned by the *caller*, not
 * the callee. This just fools gcc into not spilling into them,
 * and keeps it from doing tailcall recursion and/or using the
 * stack slots for temporaries, since they are live and "used"
 * all the way to the end of the function.
 *
 * NOTE! On x86-64, all the arguments are in registers, so this
 * only matters on a 32-bit kernel.
 */
#define asmlinkage_protect(n, ret, args...) \
	__asmlinkage_protect##n(ret, ##args)
#define __asmlinkage_protect_n(ret, args...) \
	__asm__ __volatile__ ("" : "=r" (ret) : "0" (ret), ##args)
#define __asmlinkage_protect0(ret) \
	__asmlinkage_protect_n(ret)
#define __asmlinkage_protect1(ret, arg1) \
	__asmlinkage_protect_n(ret, "g" (arg1))
#define __asmlinkage_protect2(ret, arg1, arg2) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2))
#define __asmlinkage_protect3(ret, arg1, arg2, arg3) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3))
#define __asmlinkage_protect4(ret, arg1, arg2, arg3, arg4) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4))
#define __asmlinkage_protect5(ret, arg1, arg2, arg3, arg4, arg5) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4), "g" (arg5))
#define __asmlinkage_protect6(ret, arg1, arg2, arg3, arg4, arg5, arg6) \
	__asmlinkage_protect_n(ret, "g" (arg1), "g" (arg2), "g" (arg3), \
			      "g" (arg4), "g" (arg5), "g" (arg6))

#endif /* CONFIG_X86_32 */

#ifdef __ASSEMBLY__

#define GLOBAL(name)	\
	.globl name;	\
	name:

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_ALIGNMENT_16)
#define __ALIGN		.p2align 4, 0x90
#define __ALIGN_STR	__stringify(__ALIGN)
#endif

#endif /* __ASSEMBLY__ */

#endif /* __ASM_X86_LINKAGE_H */
