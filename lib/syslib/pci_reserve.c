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
pci_reserve.c
*/

#include "pci.h"
#include "syslib.h"
#include <nucleos/sysutil.h>

/*===========================================================================*
 *				pci_reserve				     *
 *===========================================================================*/
void pci_reserve(devind)
int devind;
{
	int r;
	message m;

	m.m_type= BUSC_PCI_RESERVE;
	m.m1_i1= devind;

	r= sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_reserve: can't talk to PCI", r);

	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_reserve: got bad reply from PCI", m.m_type);
}

/*===========================================================================*
 *                              pci_reserve_ok                               *
 *===========================================================================*/
int pci_reserve_ok(devind)
int devind;
{
        int r;
        message m;

        m.m1_i1= devind;

        return(_taskcall(pci_procnr, BUSC_PCI_RESERVE, &m));
}

