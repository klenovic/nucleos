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
pci_init1.c
*/
#include <nucleos/syslib.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/sysutil.h>
#include <servers/ds/ds.h>
#include <nucleos/pci.h>

int pci_procnr= ENDPT_ANY;

/*===========================================================================*
 *				pci_init1				     *
 *===========================================================================*/
void pci_init1(name)
char *name;
{
	int r;
	u32_t u32;
	size_t len;
	kipc_msg_t m;

	r= ds_retrieve_u32("pci", &u32);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_init1: ds_retrieve_u32 failed for 'pci'", r);
	pci_procnr= u32;

	m.m_type= BUSC_PCI_INIT;
	len= strlen(name);
	if (len+1 <= sizeof(m.m_data10))
		strcpy(m.m_data10, name);
	else
	{
		len= sizeof(m.m_data10)-1;
		memcpy(m.m_data10, name, len);
		m.m_data10[len]= '\0';
	}
	r= kipc_sendrec(pci_procnr, &m);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_init1: can't talk to PCI", r);
	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_init1: got bad reply from PCI", m.m_type);
}

