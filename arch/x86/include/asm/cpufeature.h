/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_CPUFEATURE_H
#define __ASM_X86_CPUFEATURE_H

#define _CPUF_I386_FPU		0	/* FPU-x87 FPU on Chip */
#define _CPUF_I386_PSE		1	/* Page Size Extension */
#define _CPUF_I386_PGE		2	/* Page Global Enable */
#define _CPUF_I386_APIC_ON_CHIP	3	/* APIC is present on the chip */
#define _CPUF_I386_TSC		4	/* Timestamp counter present */
#define _CPUF_I386_SSEx		5	/* Support for SSE/SSE2/SSE3/SSSE3/SSE4
					 * Extensions and FXSR
					 */
#define _CPUF_I386_FXSR		6
#define _CPUF_I386_SSE		7
#define _CPUF_I386_SSE2		8
#define _CPUF_I386_SSE3		9
#define _CPUF_I386_SSSE3	10
#define _CPUF_I386_SSE4_1	11
#define _CPUF_I386_SSE4_2	12

int cpufeature(int featureno);

extern int cpu_has_tsc;		/* signal whether this cpu has time stamp register. This
				   feature was introduced by Pentium */

#endif
