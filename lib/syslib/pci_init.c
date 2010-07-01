/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/syslib.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/sysutil.h>
#include <servers/ds/ds.h>
#include <nucleos/pci.h>

int pci_procnr = ENDPT_ANY;

void pci_init(void)
{
	int r;
	u32_t u;
	size_t len;
	kipc_msg_t m;

	r = ds_retrieve_u32("pci", &u);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_init: ds_retrieve_u32 failed for 'pci'", r);

	pci_procnr = u;

	m.m_type = BUSC_PCI_INIT;

	r = kipc_sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_init: can't talk to PCI", r);
	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_init: got bad reply from PCI", m.m_type);
}

