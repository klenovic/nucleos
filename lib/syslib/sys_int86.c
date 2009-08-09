/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

/*===========================================================================*
 *                                sys_int86				     *
 *===========================================================================*/
int sys_int86(reg86p)
struct reg86u *reg86p;
{
    message m;
    int result;

    m.m1_p1= (char *)reg86p;

    result = _taskcall(SYSTASK, SYS_INT86, &m);
    return(result);
}

