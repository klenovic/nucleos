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
pci_next_dev.c
*/
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_next_dev				     *
 *===========================================================================*/
int pci_next_dev(devindp, vidp, didp)
int *devindp;
u16_t *vidp;
u16_t *didp;
{
	int r;
	kipc_msg_t m;

	m.m_type= BUSC_PCI_NEXT_DEV;
	m.m_data1= *devindp;

	r= kipc_module_call(KIPC_SENDREC, 0, pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_next_dev: can't talk to PCI", r);

	if (m.m_type == 1)
	{
		*devindp= m.m_data1;
		*vidp= m.m_data2;
		*didp= m.m_data3;
#if 0
		printk("pci_next_dev: got device %d, %04x/%04x\n", 
			*devindp, *vidp, *didp);
#endif
		return 1;
	}
	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_next_dev: got bad reply from PCI", m.m_type);

	return 0;
}

