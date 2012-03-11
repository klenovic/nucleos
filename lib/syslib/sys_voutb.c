/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_voutb				     *
 *===========================================================================*/
int sys_voutb(pvb_pairs, nr_ports)
pvb_pair_t *pvb_pairs;			/* (port,byte-value)-pairs */
int nr_ports;				/* nr of pairs to be processed */
{
    kipc_msg_t m_io;
    m_io.DIO_REQUEST = _DIO_OUTPUT | _DIO_BYTE;
    m_io.DIO_VEC_ADDR = (char *) pvb_pairs;
    m_io.DIO_VEC_SIZE = nr_ports;
    return ktaskcall(SYSTASK, SYS_VDEVIO, &m_io);
}


