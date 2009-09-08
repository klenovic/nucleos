/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_BITMAP_H
#define __NUCLEOS_BITMAP_H

#include <nucleos/kernel.h>

/* Bit map operations to manipulate bits of a simple mask variable. */
#define bit_set(mask, n)	((mask) |= (1 << (n)))
#define bit_unset(mask, n)	((mask) &= ~(1 << (n)))
#define bit_isset(mask, n)	((mask) & (1 << (n)))
#define bit_empty(mask)		((mask) = 0)
#define bit_fill(mask)		((mask) = ~0)

/* Definitions previously in kernel/const.h */
#define BITCHUNK_BITS   (sizeof(bitchunk_t) * CHAR_BIT)
#define BITMAP_CHUNKS(nr_bits) (((nr_bits)+BITCHUNK_BITS-1)/BITCHUNK_BITS)
#define MAP_CHUNK(map,bit) (map)[((bit)/BITCHUNK_BITS)]
#define CHUNK_OFFSET(bit) ((bit)%BITCHUNK_BITS))
#define GET_BIT(map,bit) ( MAP_CHUNK(map,bit) & (1 << CHUNK_OFFSET(bit) )
#define SET_BIT(map,bit) ( MAP_CHUNK(map,bit) |= (1 << CHUNK_OFFSET(bit) )
#define UNSET_BIT(map,bit) ( MAP_CHUNK(map,bit) &= ~(1 << CHUNK_OFFSET(bit) )

#endif /* __NUCLEOS_BITMAP_H */
