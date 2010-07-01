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
pci_sis.h

Created:	Nov 2001 by Philip Homburg <philip@cs.vu.nl>
*/

/* Constants are taken from pci-irq.c in the Linux kernel source */
#define SIS_ISABR_IRQ_A 0x41	/* IRQA routing */
#define SIS_ISABR_IRQ_B 0x42	/* IRQB routing */
#define SIS_ISABR_IRQ_C 0x43	/* IRQC routing */
#define SIS_ISABR_IRQ_D 0x44	/* IRQD routing */
#define 	SIS_IRQ_DISABLED	0x80
#define		SIS_IRQ_MASK		0x0F
