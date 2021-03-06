/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_reserve				     *
 *===========================================================================*/
void pci_reserve(devind)
int devind;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_RESERVE;
	m.m_data1= devind;

	r= kipc_module_call(KIPC_SENDREC, 0, pci_procnr, &m);
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
        kipc_msg_t m;

        m.m_data1= devind;

        return(ktaskcall(pci_procnr, BUSC_PCI_RESERVE, &m));
}

