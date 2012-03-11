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
rand256.h

Created:	Oct 2000 by Philip Homburg <philip@f-mnx.phicoh.com>

Provide 256-bit random numbers
*/

#define RAND256_BUFSIZE	32

void init_rand256(u8_t bits[RAND256_BUFSIZE]);
void rand256(u8_t bits[RAND256_BUFSIZE]);
