/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The system call implemented in this file:
 *   m_type:	SYS_VM_SETBUF
 *
 * The parameters for this system call are:
 *    m_data1:	Start of the buffer
 *    m_data2:	Length of the buffer
 *    m_data3:	End of main memory
 */
#include <kernel/system.h>

#define VM_DEBUG 0		/* enable/ disable debug output */

/*===========================================================================*
 *				do_vm_setbuf				     *
 *===========================================================================*/
int do_vm_setbuf(m_ptr)
kipc_msg_t *m_ptr;			/* pointer to request message */
{
	vm_base= m_ptr->m_data1;
	vm_size= m_ptr->m_data2;
	vm_mem_high= m_ptr->m_data3;

#if VM_DEBUG
	printk("do_vm_setbuf: got 0x%x @ 0x%x for 0x%x\n",
		vm_size, vm_base, vm_mem_high);
#endif

	return 0;
}
