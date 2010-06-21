/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_data3:	DIO_REQUEST	(request input or output)	
 *   m_data4:	DIO_PORT	(port to read/ write)	
 *   m_data5:	DIO_VALUE	(value to write/ return value read)	
 */

#include <kernel/system.h>
#include <nucleos/devio.h>
#include <nucleos/endpoint.h>
#include <nucleos/portio.h>

#if USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
int do_devio(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to request message */
{
    struct proc *rp;
    struct priv *privp;
    port_t port;
    struct io_range *iorp;
    int i, size, nr_io_range;
    int io_type, io_dir;

    io_type = m_ptr->DIO_REQUEST & _DIO_TYPEMASK;
    io_dir  = m_ptr->DIO_REQUEST & _DIO_DIRMASK;

	switch (io_type)
	{
	case _DIO_BYTE: size= 1; break;
	case _DIO_WORD: size= 2; break;
	case _DIO_LONG: size= 4; break;
	default: size= 4; break;	/* Be conservative */
	}

    rp= proc_addr(who_p);
    privp= priv(rp);
    if (!privp)
    {
	kprintf("no priv structure!\n");
	goto doit;
    }
    if (privp->s_flags & CHECK_IO_PORT)
    {
	switch (io_type)
	{
	case _DIO_BYTE: size= 1; break;
	case _DIO_WORD: size= 2; break;
	case _DIO_LONG: size= 4; break;
	default: size= 4; break;	/* Be conservative */
	}
	port= m_ptr->DIO_PORT;
	nr_io_range= privp->s_nr_io_range;
	for (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		if (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			break;
	}
	if (i >= nr_io_range)
	{
			kprintf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->DIO_PORT, size);
		return -EPERM;
	}
    }

doit:
    if (m_ptr->DIO_PORT & (size-1))
    {
		kprintf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->DIO_PORT, size);
	return -EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    if (io_dir == _DIO_INPUT) { 
      switch (io_type) {
	/* maybe "it" should not be called ports */
        case _DIO_BYTE: m_ptr->DIO_VALUE = inb(m_ptr->DIO_PORT); break; 
        case _DIO_WORD: m_ptr->DIO_VALUE = inw(m_ptr->DIO_PORT); break; 
        case _DIO_LONG: m_ptr->DIO_VALUE = inl(m_ptr->DIO_PORT); break;
    	default: return(-EINVAL);
      } 
    } else { 
      switch (io_type) {
	case _DIO_BYTE: outb(m_ptr->DIO_PORT, m_ptr->DIO_VALUE); break;
	case _DIO_WORD: outw(m_ptr->DIO_PORT, m_ptr->DIO_VALUE); break;
	case _DIO_LONG: outl(m_ptr->DIO_PORT, m_ptr->DIO_VALUE); break;
    	default: return(-EINVAL);
      } 
    }
    return 0;
}

#endif /* USE_DEVIO */
