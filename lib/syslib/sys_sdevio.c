/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_sdevio				     *
 *===========================================================================*/
int sys_sdevio(req, port, proc_nr, buffer, count, offset)
int req;				/* request: DIO_{IN,OUT}PUT_* */
long port; 				/* port address to read from */
endpoint_t proc_nr;			/* process where buffer is */
void *buffer;				/* pointer to buffer */
int count;				/* number of elements */
vir_bytes offset;			/* offset from grant */
{
    kipc_msg_t m_io;
    int result;

    m_io.DIO_REQUEST = req;
    m_io.DIO_PORT = port;
    m_io.DIO_VEC_ENDPT = proc_nr;
    m_io.DIO_VEC_ADDR = buffer;
    m_io.DIO_VEC_SIZE = count;
    m_io.DIO_OFFSET = offset;

    return(ktaskcall(SYSTASK, SYS_SDEVIO, &m_io));
}

