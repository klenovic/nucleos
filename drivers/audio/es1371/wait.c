/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/drivers.h>
#include <nucleos/types.h>
#include <nucleos/time.h>
#include "pci_helper.h"


int WaitBitd (int paddr, int bitno, int state, long tmout)
{
unsigned long val, mask;

	mask = 1UL << bitno;
	tmout *= 5000;

	if(state) {
    while(tmout-- > 0) {
	    if((val = pci_inl(paddr)) & mask) {
	      return 0;
	    }
	  }
	}	else {
    while(tmout-- > 0) {
	    if(!((val = pci_inl(paddr)) & mask)) {
	      return 0;
	    }
	  }
	}
	return 0;
}
