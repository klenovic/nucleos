/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef PCI_HELPER
#define PCI_HELPER

unsigned pci_inb(U16_t port);
unsigned pci_inw(U16_t port);
unsigned pci_inl(U16_t port);

void pci_outb(U16_t port, U8_t value);
void pci_outw(U16_t port, U16_t value);
void pci_outl(U16_t port, U32_t value);

#endif
