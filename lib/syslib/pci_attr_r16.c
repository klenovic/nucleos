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
pci_attr_r16.c
*/
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_attr_r16				     *
 *===========================================================================*/
u16_t pci_attr_r16(devind, port)
int devind;
int port;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_ATTR_R16;
	m.m2_i1= devind;
	m.m2_i2= port;

	r= kipc_sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_attr_r16: can't talk to PCI", r);

	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_attr_r16: got bad reply from PCI", m.m_type);

	return m.m2_l1;
}

