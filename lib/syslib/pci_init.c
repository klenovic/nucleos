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
pci_init.c
*/

#include "syslib.h"
#include <nucleos/sysutil.h>

/*===========================================================================*
 *				pci_init				     *
 *===========================================================================*/
PUBLIC void pci_init()
{
	pci_init1("");
}

