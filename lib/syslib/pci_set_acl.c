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
pci_set_acl.c
*/
#include <nucleos/syslib.h>
#include <nucleos/unistd.h>
#include <servers/rs/rs.h>
#include <servers/ds/ds.h>
#include <nucleos/sysutil.h>
#include <nucleos/pci.h>

/*===========================================================================*
 *				pci_set_acl				     *
 *===========================================================================*/
int pci_set_acl(rs_pci)
struct rs_pci *rs_pci;
{
	int r;
	cp_grant_id_t gid;
	kipc_msg_t m;
	u32_t u32;

	if (pci_procnr == ENDPT_ANY)
	{
		r= ds_retrieve_u32("pci", &u32);
		if (r != 0)
		{
			panic("syslib/" __FILE__,
				"pci_set_acl: ds_retrieve_u32 failed for 'pci'",
				r);
		}
		pci_procnr = u32;
	}


	gid= cpf_grant_direct(pci_procnr, (vir_bytes)rs_pci, sizeof(*rs_pci),
		CPF_READ);
	if (gid == -1)
	{
		printk("pci_set_acl: cpf_grant_direct failed: %d\n",
			errno);
		return -EINVAL;
	}

	m.m_type= BUSC_PCI_SET_ACL;
	m.m_data1= gid;

	r= kipc_module_call(KIPC_SENDREC, 0, pci_procnr, &m);
	cpf_revoke(gid);
	if (r != 0)
		panic("syslib/" __FILE__, "pci_set_acl: can't talk to PCI", r);

	return m.m_type;
}

