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
 *                                sys_vinl				     *
 *===========================================================================*/
int sys_vinl(pvl_pairs, nr_ports)
pvl_pair_t *pvl_pairs;			/* (port,long-value)-pairs */
int nr_ports;				/* nr of pairs to be processed */
{
    kipc_msg_t m_io;

    m_io.DIO_REQUEST = _DIO_INPUT | _DIO_LONG;
    m_io.DIO_VEC_ADDR = (char *) pvl_pairs;
    m_io.DIO_VEC_SIZE = nr_ports;
    return ktaskcall(SYSTASK, SYS_VDEVIO, &m_io);
}

