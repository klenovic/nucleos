/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
pci_via.h

Created:	Jun 2001 by Philip Homburg <philip@cs.vu.nl>
*/

#define VIA_ISABR_EL	0x54	/* Edge or level triggered */
#define		VIA_ISABR_EL_INTA	0x08	/* Edge (1) or level (0) */
#define		VIA_ISABR_EL_INTB	0x04
#define		VIA_ISABR_EL_INTC	0x02
#define		VIA_ISABR_EL_INTD	0x01

#define VIA_ISABR_IRQ_R1 0x55	/* IRQ routing 1 */
#define		VIA_ISABR_IRQ_INTD	0xf0	/* routing for INTD */
#define		VIA_ISABR_IRQ_INT0	0x0f	/* routing for INT0 */
#define VIA_ISABR_IRQ_R2 0x56	/* IRQ routing 2 */
#define		VIA_ISABR_IRQ_INTA	0xf0	/* routing for INTA */
#define		VIA_ISABR_IRQ_INTB	0x0f	/* routing for INTB */
#define VIA_ISABR_IRQ_R3 0x57	/* IRQ routing 3 */
#define		VIA_ISABR_IRQ_INTC	0xf0	/* routing for INTC */
#define		VIA_ISABR_IRQ_INT1	0x0f	/* routing for INT1 */
#define VIA_ISABR_IRQ_R4 0x58	/* IRQ routing 4 */
#define		VIA_ISABR_IRQ_INT2	0x0f	/* routing for INT2 */
