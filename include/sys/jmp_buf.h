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

#ifndef _JMP_BUF_H
#define _JMP_BUF_H

#include <nucleos/config.h>

#if defined(__ACK__)
/* as per lib/ack/rts/setjmp.e */

/* note the lack of parentheses, which would confuse 'as' */
#define JB_PC		0
#define JB_SP		JB_PC + _EM_PSIZE
#define JB_LB		JB_SP + _EM_PSIZE
#define JB_MASK		JB_LB + _EM_PSIZE
#define JB_FLAGS	JB_MASK + _EM_LSIZE

#ifdef CONFIG_X86_32
#define JB_BP		JB_LB
#endif

#elif defined(__GNUC__)

#if defined(CONFIG_X86_32) && (_WORD_SIZE == 4)
/* as per lib/gnu/rts/__setjmp.gs */
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

#endif /* __GNU__ */

#endif /* _JMP_BUF_H */
