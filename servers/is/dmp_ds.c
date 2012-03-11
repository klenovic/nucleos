/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains procedures to dump DS data structures.
 *
 * The entry points into this file are
 *   data_store_dmp:   	display DS data store contents 
 *
 * Created:
 *   Oct 18, 2005:	by Jorrit N. Herder
 */

#include "inc.h"
#include <servers/ds/store.h>

struct data_store store[NR_DS_KEYS];

static  char *s_flags_str(int flags);

/*===========================================================================*
 *				data_store_dmp				     *
 *===========================================================================*/
void data_store_dmp()
{
  struct data_store *dsp;
  int i,j, n=0, s;
  static int prev_i=0;


  printk("Data Store (DS) contents dump\n");

  if((s=getsysinfo(DS_PROC_NR, SI_DATA_STORE, store)) != 0) {
	printk("Couldn't talk to DS: %d.\n", s);
	return;
  }

  printk("slot key                  type value\n");

  for (i=prev_i; i<NR_DS_KEYS; i++) {
  	dsp = &store[i];
  	if (! dsp->ds_flags & DS_IN_USE) continue;
  	if (++n > 22) break;
  	printk("%3d  %-20s ",
		i, dsp->ds_key);
	if(dsp->ds_flags & DS_TYPE_U32) {
		printk("u32  %lu\n", dsp->ds_val.ds_val_u32);
	} else if(dsp->ds_flags & DS_TYPE_STR) {
		printk("str  \"%s\"\n", dsp->ds_val.ds_val_str);
	} else {
		printk("Bogus type\n");
	}
  }
  if (i >= NR_DS_KEYS) i = 0;
  else printk("--more--\r");
  prev_i = i;
}


static char *s_flags_str(int flags)
{
	static char str[5];
	str[0] = (flags & DS_IN_USE) ? 'U' : '-';
	str[1] = (flags & DS_PUBLIC) ? 'P' : '-';
	str[2] = '-';
	str[3] = '\0';

	return(str);
}

