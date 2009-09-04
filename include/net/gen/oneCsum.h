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
server/ip/gen/oneCsum.h
*/

#ifndef __SERVER__IP__GEN__ONECSUM_H__
#define __SERVER__IP__GEN__ONECSUM_H__

u16_t oneC_sum(u16 prev, void *data, size_t data_len);

#endif /* __SERVER__IP__GEN__ONECSUM_H__ */
