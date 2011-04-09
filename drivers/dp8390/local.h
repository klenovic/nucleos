/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
local.h
*/

#define ENABLE_WDETH 1
#define ENABLE_NE2000 1
#define ENABLE_3C503 1
#define ENABLE_PCI 1

struct dpeth;

/* 3c503.c */
int el2_probe(struct dpeth* dep);

/* dp8390.c */
u8_t inb(port_t port);
u16_t inw(port_t port);
void outb(port_t port, u8_t v);
void outw(port_t port, u16_t v);

/* ne2000.c */
int ne_probe(struct dpeth *dep);
void ne_init(struct dpeth *dep);

/* rtl8029.c */
int rtl_probe(struct dpeth *dep);

/* wdeth.c */
int wdeth_probe(struct dpeth* dep);

