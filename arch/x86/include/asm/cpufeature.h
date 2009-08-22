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

#define _CPUF_I386_PSE 1	/* Page Size Extension */
#define _CPUF_I386_PGE 2	/* Page Global Enable */

int _cpufeature(int featureno);

#endif
