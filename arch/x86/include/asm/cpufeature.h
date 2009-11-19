/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_CPUFEATURE_H
#define __ASM_X86_CPUFEATURE_H

#define _CPUF_I386_PSE			1       /* Page Size Extension */
#define _CPUF_I386_PGE			2       /* Page Global Enable */
#define _CPUF_I386_APIC_ON_CHIP		3       /* APIC is present on the chip */
#define _CPUF_I386_TSC			4       /* Timestamp counter present */

int cpufeature(int featureno);

extern int cpu_has_tsc;		/* signal whether this cpu has time stamp register. This
				   feature was introduced by Pentium */

#endif
