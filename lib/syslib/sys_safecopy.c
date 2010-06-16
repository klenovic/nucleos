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

#include <nucleos/safecopies.h>

int sys_safecopyfrom(endpoint_t src_e,
	cp_grant_id_t gr_id, vir_bytes offset,
	vir_bytes address, size_t bytes,
	int my_seg)
{
/* Transfer a block of data for which the other process has previously
 * given permission. 
 */

  kipc_msg_t copy_mess;

  copy_mess.SCP_FROM_TO = src_e;
  copy_mess.SCP_INFO = SCP_MAKEINFO(my_seg);
  copy_mess.SCP_GID = gr_id;
  copy_mess.SCP_OFFSET = (long) offset;
  copy_mess.SCP_ADDRESS = (char *) address;
  copy_mess.SCP_BYTES = (long) bytes;

  return(ktaskcall(SYSTASK, SYS_SAFECOPYFROM, &copy_mess));

}

int sys_safecopyto(endpoint_t dst_e,
	cp_grant_id_t gr_id, vir_bytes offset,
	vir_bytes address, size_t bytes,
	int my_seg)
{
/* Transfer a block of data for which the other process has previously
 * given permission. 
 */

  kipc_msg_t copy_mess;

  copy_mess.SCP_FROM_TO = dst_e;
  copy_mess.SCP_INFO = SCP_MAKEINFO(my_seg);
  copy_mess.SCP_GID = gr_id;
  copy_mess.SCP_OFFSET = (long) offset;
  copy_mess.SCP_ADDRESS = (char *) address;
  copy_mess.SCP_BYTES = (long) bytes;

  return(ktaskcall(SYSTASK, SYS_SAFECOPYTO, &copy_mess));

}
