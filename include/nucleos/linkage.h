/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* linux */
/**
 * @file include/nucleos/linkage.h
 * @brief linkage defs
 */
#ifndef __NUCLEOS_LINKAGE_H
#define __NUCLEOS_LINKAGE_H

#include <nucleos/compiler.h>
#include <asm/linkage.h>

#ifdef __cplusplus
#define CPP_ASMLINKAGE extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#ifndef asmlinkage
#define asmlinkage CPP_ASMLINKAGE
#endif

#ifndef asmregparm
# define asmregparm
#endif

#define __page_aligned_data	__section(.data.page_aligned) __aligned(PAGE_SIZE)
#define __page_aligned_bss	__section(.bss.page_aligned) __aligned(PAGE_SIZE)

/*
 * For assembly routines.
 *
 * Note when using these that you must specify the appropriate
 * alignment directives yourself
 */
#define __PAGE_ALIGNED_DATA	.section ".data.page_aligned", "aw"
#define __PAGE_ALIGNED_BSS	.section ".bss.page_aligned", "aw"

/*
 * This is used by architectures to keep arguments on the stack
 * untouched by the compiler by keeping them live until the end.
 * The argument stack may be owned by the assembly-language
 * caller, not the callee, and gcc doesn't always understand
 * that.
 *
 * We have the return value, and a maximum of six arguments.
 *
 * This should always be followed by a "return ret" for the
 * protection to work (ie no more work that the compiler might
 * end up needing stack temporaries for).
 */
/* Assembly files may be compiled with -traditional .. */
#ifndef __ASSEMBLY__
#ifndef asmlinkage_protect
# define asmlinkage_protect(n, ret, args...)	do { } while (0)
#endif
#endif

#ifndef __ALIGN
#define __ALIGN		.align 4,0x90
#define __ALIGN_STR	".align 4,0x90"
#endif

#ifdef __ASSEMBLY__

#ifndef LINKER_SCRIPT
#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR

#ifndef ENTRY
#define ENTRY(name)	\
	.globl name;	\
	ALIGN;		\
	name:
#endif
#endif /* LINKER_SCRIPT */

#ifndef WEAK
#define WEAK(name)	\
	.weak name;	\
	name:
#endif

#ifndef END
#define END(name) \
	.size name, .-name
#endif

/* If symbol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC for the benefit of
 * static analysis tools such as stack depth analyzer.
 */
#ifndef ENDPROC
#define ENDPROC(name)		\
	.type name, @function;	\
	END(name)
#endif

#endif /* __ASSEMBLY__ */

#define NORET_TYPE    /**/
#define ATTRIB_NORET	__attribute__((noreturn))
#define NORET_AND	noreturn,

#endif /* !__NUCLEOS_LINKAGE_H */
