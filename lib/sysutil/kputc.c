/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* A server must occasionally print some message.  It uses a simple version of 
 * printf() found in the system lib that calls kputc() to output characters.
 * Printing is done with a call to the kernel, and not by going through FS_PROC_NR.
 *
 * This routine can only be used by servers and device drivers.  The kernel
 * must define its own kputc(). Note that the log driver also defines its own 
 * kputc() to directly call the TTY instead of going through this library.
 */

#include "sysutil.h"

static char print_buf[DIAG_BUFSIZE];	/* output is buffered here */

/*===========================================================================*
 *				kputc					     *
 *===========================================================================*/
void kputc(c)
int c;
{
/* Accumulate another character.  If 0 or buffer full, print it. */
  static int buf_count;		/* # characters in the buffer */
  kipc_msg_t m;

  if ((c == 0 && buf_count > 0) || buf_count == sizeof(print_buf)) {
	sys_sysctl(SYSCTL_CODE_DIAG, print_buf, buf_count);
	buf_count = 0;
  }
  if (c != 0) { 
        
        /* Append a single character to the output buffer. */
  	print_buf[buf_count++] = c;
  }
}
