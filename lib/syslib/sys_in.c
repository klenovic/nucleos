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
 *                                sys_in				     *
 *===========================================================================*/
PUBLIC int sys_in(port, value, type)
int port; 				/* port address to read from */
unsigned long *value;			/* pointer where to store value */
int type;				/* byte, word, long */
{
    message m_io;
    int result;

    m_io.DIO_REQUEST = _DIO_INPUT | type;
    m_io.DIO_PORT = port;

    result = _taskcall(SYSTASK, SYS_DEVIO, &m_io);
    *value = m_io.DIO_VALUE;
    return(result);
}

