/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_READBIOS
 *
 * The parameters for this kernel call are:
 *   message->RDB_SIZE		number of bytes to copy
 *   message->RDB_ADDR		absolute address in BIOS area
 *   message->RDB_BUF		buffer address in requesting process
 */

#include <kernel/system.h>
#include <nucleos/type.h>

int do_readbios(kipc_msg_t *m_ptr)
{
	struct vir_addr src, dst;

	src.segment = BIOS_SEG;
	dst.segment = D;
	src.offset = m_ptr->RDB_ADDR;
	dst.offset = (vir_bytes) m_ptr->RDB_BUF;
	src.proc_nr_e = ENDPT_NONE;
	dst.proc_nr_e = m_ptr->m_source;

	return virtual_copy_vmcheck(&src, &dst, m_ptr->RDB_SIZE);
}
