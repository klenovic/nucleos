/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Global variables. */

/* Parameters needed to keep diagnostics at IS. */
#define DIAG_BUF_SIZE 1024
extern char diag_buf[DIAG_BUF_SIZE];	/* buffer for messages */
extern int diag_next;			/* next index to be written */
extern int diag_size;			/* size of all messages */

/* Flag to indicate system-wide panic. */
extern int sys_panic;		/* if set, shutdown can be done */

/* The parameters of the call are kept here. */
extern kipc_msg_t m_in;		/* the input message itself */
extern kipc_msg_t m_out;		/* the output message used for reply */
extern int who_e;		/* caller's proc number */
extern int callnr;		/* system call number */
extern int dont_reply;		/* normally 0; set to 1 to inhibit reply */

