/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "sysutil.h"
#include <timers.h>

#define COM1_BASE       0x3F8
#define COM1_THR        (COM1_BASE + 0)
#define         LSR_THRE        0x20
#define COM1_LSR        (COM1_BASE + 5)

/*===========================================================================*
 *                               ser_putc			    	     *
 *===========================================================================*/
PUBLIC void ser_putc(char c)
{
        unsigned long b;
        int i;
        int lsr, thr;
  
        lsr= COM1_LSR;
        thr= COM1_THR;
        for (i= 0; i<10000; i++)
        {
                sys_inb(lsr, &b);
                if (b & LSR_THRE)
                        break;
        }
        sys_outb(thr, c);
}
