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
rtl8029.h

Created:	Sep 2003 by Philip Homburg <philip@f-mnx.phicoh.com>
*/

/* Bits in dp_cr */
#define CR_PS_P3	0xC0	/* Register Page 3                   */

#define inb_reg3(dep, reg)	(inb (dep->de_dp8390_port+reg))
#define outb_reg3(dep, reg, data) (outb(dep->de_dp8390_port+reg, data))
