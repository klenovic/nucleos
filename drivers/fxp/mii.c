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
ibm/mii.c

Created:	Nov 2004 by Philip Homburg <philip@f-mnx.phicoh.com>

Media Independent (Ethernet) Interface functions
*/

#include <nucleos/drivers.h>
#include "mii.h"

/*===========================================================================*
 *				mii_print_stat_speed			     *
 *===========================================================================*/
void mii_print_stat_speed(stat, extstat)
u16_t stat;
u16_t extstat;
{
	int fs, ft;

	fs= 1;
	if (stat & MII_STATUS_EXT_STAT)
	{
		if (extstat & (MII_ESTAT_1000XFD | MII_ESTAT_1000XHD |
			MII_ESTAT_1000TFD | MII_ESTAT_1000THD))
		{
			printk("1000 Mbps: ");
			fs= 0;
			ft= 1;
			if (extstat & (MII_ESTAT_1000XFD | MII_ESTAT_1000XHD))
			{
				ft= 0;
				printk("X-");
				switch(extstat &
					(MII_ESTAT_1000XFD|MII_ESTAT_1000XHD))
				{
				case MII_ESTAT_1000XFD:	printk("FD"); break;
				case MII_ESTAT_1000XHD:	printk("HD"); break;
				default:		printk("FD/HD"); break;
				}
			}
			if (extstat & (MII_ESTAT_1000TFD | MII_ESTAT_1000THD))
			{
				if (!ft)
					printk(", ");
				ft= 0;
				printk("T-");
				switch(extstat &
					(MII_ESTAT_1000TFD|MII_ESTAT_1000THD))
				{
				case MII_ESTAT_1000TFD:	printk("FD"); break;
				case MII_ESTAT_1000THD:	printk("HD"); break;
				default:		printk("FD/HD"); break;
				}
			}
		}
	}
	if (stat & (MII_STATUS_100T4 |
		MII_STATUS_100XFD | MII_STATUS_100XHD |
		MII_STATUS_100T2FD | MII_STATUS_100T2HD))
	{
		if (!fs)
			printk(", ");
		fs= 0;
		printk("100 Mbps: ");
		ft= 1;
		if (stat & MII_STATUS_100T4)
		{
			printk("T4");
			ft= 0;
		}
		if (stat & (MII_STATUS_100XFD | MII_STATUS_100XHD))
		{
			if (!ft)
				printk(", ");
			ft= 0;
			printk("TX-");
			switch(stat & (MII_STATUS_100XFD|MII_STATUS_100XHD))
			{
			case MII_STATUS_100XFD:	printk("FD"); break;
			case MII_STATUS_100XHD:	printk("HD"); break;
			default:		printk("FD/HD"); break;
			}
		}
		if (stat & (MII_STATUS_100T2FD | MII_STATUS_100T2HD))
		{
			if (!ft)
				printk(", ");
			ft= 0;
			printk("T2-");
			switch(stat & (MII_STATUS_100T2FD|MII_STATUS_100T2HD))
			{
			case MII_STATUS_100T2FD:	printk("FD"); break;
			case MII_STATUS_100T2HD:	printk("HD"); break;
			default:		printk("FD/HD"); break;
			}
		}
	}
	if (stat & (MII_STATUS_10FD | MII_STATUS_10HD))
	{
		if (!fs)
			printk(", ");
		printk("10 Mbps: ");
		fs= 0;
		printk("T-");
		switch(stat & (MII_STATUS_10FD|MII_STATUS_10HD))
		{
		case MII_STATUS_10FD:	printk("FD"); break;
		case MII_STATUS_10HD:	printk("HD"); break;
		default:		printk("FD/HD"); break;
		}
	}
}

/*===========================================================================*
 *				mii_print_techab			     *
 *===========================================================================*/
void mii_print_techab(techab)
u16_t techab;
{
	int fs, ft;

	if ((techab & MII_ANA_SEL_M) != MII_ANA_SEL_802_3)
	{
		printk("strange selector 0x%x, value 0x%x",
			techab & MII_ANA_SEL_M,
			(techab & MII_ANA_TAF_M) >> MII_ANA_TAF_S);
		return;
	}
	fs= 1;
	if (techab & (MII_ANA_100T4 | MII_ANA_100TXFD | MII_ANA_100TXHD))
	{
		printk("100 Mbps: ");
		fs= 0;
		ft= 1;
		if (techab & MII_ANA_100T4)
		{
			printk("T4");
			ft= 0;
		}
		if (techab & (MII_ANA_100TXFD | MII_ANA_100TXHD))
		{
			if (!ft)
				printk(", ");
			ft= 0;
			printk("TX-");
			switch(techab & (MII_ANA_100TXFD|MII_ANA_100TXHD))
			{
			case MII_ANA_100TXFD:	printk("FD"); break;
			case MII_ANA_100TXHD:	printk("HD"); break;
			default:		printk("FD/HD"); break;
			}
		}
	}
	if (techab & (MII_ANA_10TFD | MII_ANA_10THD))
	{
		if (!fs)
			printk(", ");
		printk("10 Mbps: ");
		fs= 0;
		printk("T-");
		switch(techab & (MII_ANA_10TFD|MII_ANA_10THD))
		{
		case MII_ANA_10TFD:	printk("FD"); break;
		case MII_ANA_10THD:	printk("HD"); break;
		default:		printk("FD/HD"); break;
		}
	}
	if (techab & MII_ANA_PAUSE_SYM)
	{
		if (!fs)
			printk(", ");
		fs= 0;
		printk("pause(SYM)");
	}
	if (techab & MII_ANA_PAUSE_ASYM)
	{
		if (!fs)
			printk(", ");
		fs= 0;
		printk("pause(ASYM)");
	}
	if (techab & MII_ANA_TAF_RES)
	{
		if (!fs)
			printk(", ");
		fs= 0;
		printk("0x%x", (techab & MII_ANA_TAF_RES) >> MII_ANA_TAF_S);
	}
}
