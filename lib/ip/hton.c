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
hton.c
*/

#include <nucleos/types.h>
#include <net/hton.h>

u16_t _tmp;
u32_t _tmp_l;

u16_t (htons)(u16_t x) { return HTONS(x); }
u16_t (ntohs)(u16_t x) { return NTOHS(x); }
u32_t (htonl)(u32_t x) { return HTONL(x); }
u32_t (ntohl)(u32_t x) { return NTOHL(x); }

