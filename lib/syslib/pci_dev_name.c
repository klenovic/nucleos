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
pci_dev_name.c
*/

#include "pci.h"
#include "syslib.h"
#include <nucleos/sysutil.h>

/*===========================================================================*
 *				pci_dev_name				     *
 *===========================================================================*/
char *pci_dev_name(vid, did)
u16_t vid;
u16_t did;
{
	static char name[80];	/* We need a better interface for this */

	int r;
	cp_grant_id_t gid;
	message m;

	gid= cpf_grant_direct(pci_procnr, (vir_bytes)name, sizeof(name),
		CPF_WRITE);
	if (gid == -1)
	{
		printf("pci_dev_name: cpf_grant_direct failed: %d\n",
			errno);
		return NULL;
	}

	m.m_type= BUSC_PCI_DEV_NAME_S;
	m.m7_i1= vid;
	m.m7_i2= did;
	m.m7_i3= sizeof(name);
	m.m7_i4= gid;

	r= kipc_sendrec(pci_procnr, &m);
	cpf_revoke(gid);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_dev_name: can't talk to PCI", r);

	if (m.m_type == -ENOENT)
	{
		printf("pci_dev_name: got no name\n");
		return NULL;	/* No name for this device */
	}
	if (m.m_type != 0)
		panic("syslib/" __FILE__, "pci_dev_name: got bad reply from PCI", m.m_type);

	name[sizeof(name)-1]= '\0';	/* Make sure that the string is NUL
					 * terminated.
					 */

#if DEBUG
	printf("pci_dev_name: got name %s\n", name);
#endif
	return name;
}

