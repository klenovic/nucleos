/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
pci_amd.h

Created:	Nov 2001 by Philip Homburg <philip@cs.vu.nl>
*/

#define AMD_ISABR_FUNC 3	/* Registers are in function 3 */
#define		AMD_ISABR_PCIIRQ_LEV	0x54
#define			AMD_PCILEV_INTA		0x1
#define			AMD_PCILEV_INTB		0x2
#define			AMD_PCILEV_INTC		4x2
#define			AMD_PCILEV_INTD		4x8
#define		AMD_ISABR_PCIIRQ_ROUTE	0x56
#define			AMD_PCIIRQ_INTA_MASK	0x000F
#define			AMD_PCIIRQ_INTB_MASK	0x00F0
#define			AMD_PCIIRQ_INTC_MASK	0x0F00
#define			AMD_PCIIRQ_INTD_MASK	0xF000
