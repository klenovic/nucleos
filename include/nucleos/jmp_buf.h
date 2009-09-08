/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file is intended for use by program code (possibly written in
 * assembly) that needs to manipulate a jmp_buf or sigjmp_buf. The JB_*
 * values are byte offsets into the jmp_buf and sigjmp_buf structures.
 */

#ifndef __NUCLEOS_JMP_BUF_H
#define __NUCLEOS_JMP_BUF_H

#ifdef __GNUC__

#ifdef CONFIG_X86_32
/* as per lib/gnu/rts/__setjmp.S */
#define JB_FLAGS	0
#define JB_MASK		4
#define JB_PC		8
#define JB_SP		12
#define JB_BP		16
#define JB_BX		20
#define JB_CX		24
#define JB_DX		28
#define JB_SI		32
#define JB_DI		36
#endif /* CONFIG_X86_32 */

#endif /* __GNUC__ */

#endif /* __NUCLEOS_JMP_BUF_H */
