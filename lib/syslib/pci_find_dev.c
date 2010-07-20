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
pci_find_dev.c
*/
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_find_dev				     *
 *===========================================================================*/
int pci_find_dev(bus, dev, func, devindp)
u8_t bus;
u8_t dev;
u8_t func;
int *devindp;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_FIND_DEV;
	m.m_data1= bus;
	m.m_data2= dev;
	m.m_data3= func;

	r= kipc_sendrec(pci_procnr, &m, 0);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_find_dev: can't talk to PCI", r);

	if (m.m_type == 1)
	{
		*devindp= m.m_data1;
		printf("pci_find_dev: got device %d for %d.%d.%d\n",
			*devindp, bus, dev, func);
		return 1;
	}
	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_find_dev: got bad reply from PCI", m.m_type);

	printf("pci_find_dev: got nothing\n");
	return 0;
}

