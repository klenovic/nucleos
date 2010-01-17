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
ethera2n.c

Convert an ASCII string with an ethernet address into a struct ether_addr.

Created:	Nov 17, 1992 by Philip Homburg
*/

#include <nucleos/types.h>
#include <stdlib.h>
#include <net/netlib.h>
#include <net/ether.h>
#include <net/if_ether.h>

struct ether_addr *ether_aton(s)
const char *s;
{
	static struct ether_addr ea;

	int i;
	long v;
	char *check;

	if (s == NULL)
		return NULL;

	for (i=0; i<6; i++)
	{
		v= strtol(s, &check, 16);
		if (v<0 || v>255)
			return NULL;
		if ((i<5 && check[0] != ':') || (i == 5 && check[0] != '\0'))
			return NULL;
		ea.ea_addr[i]= v;
		s= check+1;
	}
	return &ea;
}
