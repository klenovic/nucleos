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
minix/portio.h

Created:	Jan 15, 1992 by Philip Homburg
*/

#ifndef _NUCLEOS_PORTIO_H
#define _NUCLEOS_PORTIO_H

#include <nucleos/types.h>

unsigned inb(u16 _port);
unsigned inw(u16 _port);
unsigned inl(u32 _port);
void outb(u16 _port, u8 _value);
void outw(u16 _port, u16 _value);
void outl(u16 _port, u32 _value);
void insb(u16 _port, void *_buf, size_t _count);
void insw(u16 _port, void *_buf, size_t _count);
void insl(u16 _port, void *_buf, size_t _count);
void outsb(u16 _port, void *_buf, size_t _count);
void outsw(u16 _port, void *_buf, size_t _count);
void outsl(u16 _port, void *_buf, size_t _count);
void intr_disable(void);
void intr_enable(void);

#endif /* _NUCLEOS_PORTIO_H */
