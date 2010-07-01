/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/string.h>

#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_getinfo				     *
 *===========================================================================*/
int sys_getinfo(request, ptr, len, ptr2, len2)
int request; 				/* system info requested */
void *ptr;				/* pointer where to store it */
int len;				/* max length of value to get */
void *ptr2;				/* second pointer */
int len2;				/* length or process nr */ 
{
    kipc_msg_t m;

    m.I_REQUEST = request;
    m.I_ENDPT = ENDPT_SELF;			/* always store values at caller */
    m.I_VAL_PTR = ptr;
    m.I_VAL_LEN = len;
    m.I_VAL_PTR2 = ptr2;
    m.I_VAL_LEN2_E = len2;

    return(ktaskcall(SYSTASK, SYS_GETINFO, &m));
}
