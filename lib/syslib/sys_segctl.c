/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                               sys_segctl				     *    
 *===========================================================================*/
int sys_segctl(index, seg, off, phys, size)
int *index;				/* return index of remote segment */
u16_t *seg;				/* return segment selector here */
vir_bytes *off;				/* return offset in segment here */
phys_bytes phys;			/* physical address to convert */
vir_bytes size;				/* size of segment */
{
    message m;
    int s;
    m.SEG_PHYS = phys;
    m.SEG_SIZE = size;
    s = _taskcall(SYSTASK, SYS_SEGCTL, &m);
    *index = (int) m.SEG_INDEX;
    *seg = (u16_t) m.SEG_SELECT;
    *off = (vir_bytes) m.SEG_OFFSET;
    return s;
}


