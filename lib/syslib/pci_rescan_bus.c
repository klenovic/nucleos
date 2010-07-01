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
pci_rescan_bus.c
*/
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_rescan_bus				     *
 *===========================================================================*/
void pci_rescan_bus(busnr)
u8_t busnr;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_RESCAN;
	m.m_data1= busnr;

	r= kipc_sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_rescan_bus: can't talk to PCI", r);

	if (m.m_type != 0)
	{
		panic("syslib/" __FILE__, "pci_rescan_bus: got bad reply from PCI",
			m.m_type);
	}
}

