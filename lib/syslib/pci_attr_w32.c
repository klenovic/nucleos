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
pci_attr_w32.c
*/
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_attr_w32				     *
 *===========================================================================*/
void pci_attr_w32(devind, port, value)
int devind;
int port;
u32_t value;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_ATTR_W32;
	m.m_data1= devind;
	m.m_data2= port;
	m.m_data4= value;

	r= kipc_sendrec(pci_procnr, &m, 0);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_attr_w32: can't talk to PCI", r);

	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_attr_w32: got bad reply from PCI", m.m_type);
}

