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
pci_del_acl.c
*/

#include "pci.h"
#include "syslib.h"
#include <unistd.h>
#include <servers/rs/rs.h>
#include <servers/ds/ds.h>
#include <nucleos/sysutil.h>

/*===========================================================================*
 *				pci_del_acl				     *
 *===========================================================================*/
int pci_del_acl(proc_nr)
endpoint_t proc_nr;
{
	int r;
	message m;
	u32_t u32;

	if (pci_procnr == ANY)
	{
		r= ds_retrieve_u32("pci", &u32);
		if (r != 0)
		{
			panic("syslib/" __FILE__,
				"pci_del_acl: _pm_findproc failed for 'pci'",
				r);
		}
		pci_procnr = u32;
	}


	m.m_type= BUSC_PCI_DEL_ACL;
	m.m1_i1= proc_nr;

	r= sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_del_acl: can't talk to PCI", r);

	return m.m_type;
}

