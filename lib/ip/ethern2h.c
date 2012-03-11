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
ethern2h.c

Created:	Nov 12, 1992 by Philip Homburg
*/

#include <nucleos/types.h>
#include <stdio.h>
#include <nucleos/string.h>
#include <net/ether.h>
#include <net/if_ether.h>

int
ether_ntohost(hostname, e)
char *hostname;
struct ether_addr *e;
{
	FILE *etherf;
	char b[256];
	struct ether_addr e_tmp;

	etherf= fopen(_PATH_ETHERS, "r");
	if (etherf == NULL)
		return 1;

	while(fgets(b, sizeof(b), etherf) != NULL)
	{
		if (ether_line(b, &e_tmp, hostname) == 0 && 
		memcmp(&e_tmp, e, sizeof(e_tmp)) == 0)
		{
			fclose(etherf);
			return 0;
		}
	}
	fclose(etherf);
	return 1;
}
