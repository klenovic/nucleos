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
rand256.c

Created:	Oct 2000 by Philip Homburg <philip@f-mnx.phicoh.com>

Generate 256-bit random numbers 
*/

#include "../sha2.h"
#include "../inet.h"
#include "rand256.h"

static u32_t base_bits[8];

void init_rand256(bits)
u8_t bits[32];
{
	memcpy(base_bits, bits, sizeof(base_bits));
}

void rand256(bits)
u8_t bits[32];
{
	u32_t a;
	SHA256_CTX ctx;

	a= ++base_bits[0];
	if (a == 0)
		base_bits[1]++;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (unsigned char *)base_bits, sizeof(base_bits));
	SHA256_Final(bits, &ctx);
}
