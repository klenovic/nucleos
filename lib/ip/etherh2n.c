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
etherh2n.c

Created:	May 20, 1992 by Philip Homburg
*/

#include <stdio.h>
#include <nucleos/string.h>
#include <net/if_ether.h>

int
ether_hostton(hostname, e)
char *hostname;
struct ether_addr *e;
{
	FILE *etherf;
	char b[256], hn[256];

	etherf= fopen(_PATH_ETHERS, "r");
	if (etherf == NULL)
		return 1;

	while(fgets(b, sizeof(b), etherf) != NULL)
	{
		if (ether_line(b, e, hn) == 0 && strcmp(hn, hostname) == 0)
		{
			fclose(etherf);
			return 0;
		}
	}
	fclose(etherf);
	return 1;
}
