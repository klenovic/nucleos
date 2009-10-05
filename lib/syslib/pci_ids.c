/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
pci_ids.c
*/

#include "pci.h"
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>

/*===========================================================================*
 *				pci_ids					     *
 *===========================================================================*/
void pci_ids(devind, vidp, didp)
int devind;
u16_t *vidp;
u16_t *didp;
{
	int r;
	message m;

	m.m_type= BUSC_PCI_IDS;
	m.m1_i1= devind;

	r= kipc_sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_ids: can't talk to PCI", r);

	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_ids: got bad reply from PCI", m.m_type);
	*vidp= m.m1_i1;
	*didp= m.m1_i2;
	printf("pci_ids: %04x/%04x\n", *vidp, *didp);
}

