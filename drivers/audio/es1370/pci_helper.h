/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef PCI_HELPER
#define PCI_HELPER

unsigned pci_inb(u16 port);
unsigned pci_inw(u16 port);
unsigned pci_inl(u16 port);

void pci_outb(u16 port, u8 value);
void pci_outw(u16 port, u16 value);
void pci_outl(u16 port, u32 value);

#endif
