/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	oneC_sum() - One complement's checksum		Author: Kees J. Bot
 *								8 May 1995
 * See RFC 1071, "Computing the Internet checksum"
 */

#include <nucleos/types.h>
#include <net/oneCsum.h>

u16_t oneC_sum(u16 prev, void *data, size_t size)
{
	u8_t *dptr;
	size_t n;
	u16_t word;
	u32_t sum;
	int swap= 0;

	sum= prev;
	dptr= data;
	n= size;

	swap= ((size_t) dptr & 1);
	if (swap) {
		sum= ((sum & 0xFF) << 8) | ((sum & 0xFF00) >> 8);
		if (n > 0) {
			((u8_t *) &word)[0]= 0;
			((u8_t *) &word)[1]= dptr[0];
			sum+= (u32_t) word;
			dptr+= 1;
			n-= 1;
		}
	}

	while (n >= 8) {
		sum+= (u32_t) ((u16_t *) dptr)[0]
		    + (u32_t) ((u16_t *) dptr)[1]
		    + (u32_t) ((u16_t *) dptr)[2]
		    + (u32_t) ((u16_t *) dptr)[3];
		dptr+= 8;
		n-= 8;
	}

	while (n >= 2) {
		sum+= (u32_t) ((u16_t *) dptr)[0];
		dptr+= 2;
		n-= 2;
	}

	if (n > 0) {
		((u8_t *) &word)[0]= dptr[0];
		((u8_t *) &word)[1]= 0;
		sum+= (u32_t) word;
	}

	sum= (sum & 0xFFFF) + (sum >> 16);
	if (sum > 0xFFFF) sum++;

	if (swap) {
		sum= ((sum & 0xFF) << 8) | ((sum & 0xFF00) >> 8);
	}
	return sum;
}
