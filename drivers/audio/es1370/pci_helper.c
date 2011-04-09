/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* best viewed with tabsize 4 */

#include <nucleos/drivers.h>

#include <stdlib.h>
#include <stdio.h>
#include <nucleos/string.h>
#include <nucleos/sysutil.h>
#include <nucleos/errno.h>


#include "pci_helper.h"

#include "es1370.h"

/*===========================================================================*
 *			helper functions for I/O										 *
 *===========================================================================*/
unsigned pci_inb(u16 port) {
	u32_t value;
	int s;
	if ((s=sys_inb(port, (unsigned long*)&value)) != 0)
		printk("%s: warning, sys_inb failed: %d\n", DRIVER_NAME, s);
	return value;
}


unsigned pci_inw(u16 port) {
	u32_t value;
	int s;
	if ((s=sys_inw(port, (unsigned long*)&value)) != 0)
		printk("%s: warning, sys_inw failed: %d\n", DRIVER_NAME, s);
	return value;
}


unsigned pci_inl(u16 port) {
	u32 value;
	int s;
	if ((s=sys_inl(port, (unsigned long*)&value)) != 0)
		printk("%s: warning, sys_inl failed: %d\n", DRIVER_NAME, s);
	return value;
}


void pci_outb(u16 port, u8 value) {
	int s;
	if ((s=sys_outb(port, value)) != 0)
		printk("%s: warning, sys_outb failed: %d\n", DRIVER_NAME, s);
}


void pci_outw(u16 port, u16 value) {
	int s;
	if ((s=sys_outw(port, value)) != 0)
		printk("%s: warning, sys_outw failed: %d\n", DRIVER_NAME, s);
}


void pci_outl(u16 port, u32 value) {
	int s;
	if ((s=sys_outl(port, value)) != 0)
		printk("%s: warning, sys_outl failed: %d\n", DRIVER_NAME, s);
}

