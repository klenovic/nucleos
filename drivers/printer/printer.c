/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains the printer driver. It is a fairly simple driver,
 * supporting only one printer.  Characters that are written to the driver
 * are written to the printer without any changes at all.
 *
 * Changes:
 *	May 07, 2004	fix: wait until printer is ready  (Jorrit N. Herder)
 *	May 06, 2004	printer driver moved to user-space  (Jorrit N. Herder) 
 *
 * The valid messages and their parameters are:
 *
 *   DEV_OPEN:	initializes the printer
 *   DEV_CLOSE:	does nothing
 *   HARD_INT:	interrupt handler has finished current chunk of output
 *   DEV_WRITE:	a process wants to write on a terminal
 *   CANCEL:	terminate a previous incomplete system call immediately
 *
 *    m_type      TTY_LINE   IO_ENDPT    COUNT    ADDRESS
 * |-------------+---------+---------+---------+---------|
 * | DEV_OPEN    |         |         |         |         |
 * |-------------+---------+---------+---------+---------|
 * | DEV_CLOSE   |         | proc nr |         |         |
 * -------------------------------------------------------
 * | HARD_INT    |         |         |         |         |
 * |-------------+---------+---------+---------+---------|
 * | SYS_EVENT   |         |         |         |         |
 * |-------------+---------+---------+---------+---------|
 * | DEV_WRITE   |minor dev| proc nr |  count  | buf ptr |
 * |-------------+---------+---------+---------+---------|
 * | CANCEL      |minor dev| proc nr |         |         |
 * -------------------------------------------------------
 * 
 * Note: since only 1 printer is supported, minor dev is not used at present.
 */
#include <nucleos/endpoint.h>
#include <nucleos/drivers.h>

/* Control bits (in port_base + 2).  "+" means positive logic and "-" means
 * negative logic.  Most of the signals are negative logic on the pins but
 * many are converted to positive logic in the ports.  Some manuals are
 * misleading because they only document the pin logic.
 *
 *	+0x01	Pin 1	-Strobe
 *	+0x02	Pin 14	-Auto Feed
 *	-0x04	Pin 16	-Initialize Printer
 *	+0x08	Pin 17	-Select Printer
 *	+0x10	IRQ7 Enable
 *
 * Auto Feed and Select Printer are always enabled. Strobe is enabled briefly
 * when characters are output.  Initialize Printer is enabled briefly when
 * the task is started.  IRQ7 is enabled when the first character is output
 * and left enabled until output is completed (or later after certain
 * abnormal completions).
 */
#define ASSERT_STROBE   0x1D	/* strobe a character to the interface */
#define NEGATE_STROBE   0x1C	/* enable interrupt on interface */
#define PR_SELECT          0x0C	/* select printer bit */
#define INIT_PRINTER    0x08	/* init printer bits */

/* Status bits (in port_base + 2).
 *
 *	-0x08	Pin 15	-Error
 *	+0x10	Pin 13	+Select Status
 *	+0x20	Pin 12	+Out of Paper
 *	-0x40	Pin 10	-Acknowledge
 *	-0x80	Pin 11	+Busy
 */
#define BUSY_STATUS     0x10	/* printer gives this status when busy */
#define NO_PAPER        0x20	/* status bit saying that paper is out */
#define NORMAL_STATUS   0x90	/* printer gives this status when idle */
#define ON_LINE         0x10	/* status bit saying that printer is online */
#define STATUS_MASK	0xB0	/* mask to filter out status bits */ 

#define MAX_ONLINE_RETRIES 120  /* about 60s: waits 0.5s after each retry */

/* Centronics interface timing that must be met by software (in microsec).
 *
 * Strobe length:	0.5u to 100u (not sure about the upper limit).
 * Data set up:		0.5u before strobe.
 * Data hold:		0.5u after strobe.
 * Init pulse length:	over 200u (not sure).
 *
 * The strobe length is about 50u with the code here and function calls for
 * sys_outb() - not much to spare.  The 0.5u minimums will not be violated 
 * with the sys_outb() messages exchanged.
 */

static int caller;		/* process to tell when printing done (VFS_PROC_NR) */
static int revive_pending;	/* set to true if revive is pending */
static int revive_status;	/* revive status */
static int done_status;	/* status of last output completion */
static int oleft;		/* bytes of output left in obuf */
static unsigned char obuf[128];	/* output buffer */
static unsigned char *optr;		/* ptr to next char in obuf to print */
static int orig_count;		/* original byte count */
static int port_base;		/* I/O port for printer */
static int proc_nr;		/* user requesting the printing */
static cp_grant_id_t grant_nr;	/* grant on which print happens */
static int user_left;		/* bytes of output left in user buf */
static vir_bytes user_vir_g;	/* start of user buf (address or grant) */
static vir_bytes user_vir_d;	/* offset in user buf */
static int user_safe;		/* address or grant? */
static int writing;		/* nonzero while write is in progress */
static int irq_hook_id;	/* id of irq hook at kernel */

extern int errno;		/* error number */

static void do_cancel(kipc_msg_t *m_ptr);
static void output_done(void);
static void do_write(kipc_msg_t *m_ptr, int safe);
static void do_status(kipc_msg_t *m_ptr);
static void prepare_output(void);
static void do_initialize(void);
static void reply(int code,int replyee,int proc,int status);
static void do_printer_output(void);
static void do_signal(void);


/*===========================================================================*
 *				printer_task				     *
 *===========================================================================*/
void main(void)
{
/* Main routine of the printer task. */
  kipc_msg_t pr_mess;		/* buffer for all incoming messages */
  struct sigaction sa;
  int s;

  /* Install signal handlers. Ask PM to transform signal into message. */
  sa.sa_handler = SIG_MESS;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGTERM,&sa,NULL)<0) panic("PRN","sigaction failed", errno);
  
  while (TRUE) {
	kipc_module_call(KIPC_RECEIVE, 0, ENDPT_ANY, &pr_mess);

	if (is_notify(pr_mess.m_type)) {
		switch (_ENDPOINT_P(pr_mess.m_source)) {
			case HARDWARE:
				do_printer_output();
				break;
			case RS_PROC_NR:
				kipc_module_call(KIPC_NOTIFY, 0, pr_mess.m_source, 0);
				break;
			case PM_PROC_NR:
				do_signal();
				break;
			default:
				reply(KCNR_TASK_REPLY, pr_mess.m_source,
						pr_mess.IO_ENDPT, -EINVAL);
		}
		continue;
	}

	switch(pr_mess.m_type) {
	    case DEV_OPEN:
                 do_initialize();		/* initialize */
	        /* fall through */
	    case DEV_CLOSE:
		reply(KCNR_TASK_REPLY, pr_mess.m_source, pr_mess.IO_ENDPT, 0);
		break;
	    case DEV_WRITE_S:	do_write(&pr_mess, 1);	break;
	    case DEV_STATUS:	do_status(&pr_mess);	break;
	    case CANCEL:	do_cancel(&pr_mess);	break;
	    default:
		reply(KCNR_TASK_REPLY, pr_mess.m_source, pr_mess.IO_ENDPT, -EINVAL);
	}
  }
}


/*===========================================================================*
 *				 do_signal	                             *
 *===========================================================================*/
static void do_signal()
{
  sigset_t sigset;

  if (getsigset(&sigset) != 0) return;
  
  /* Expect a SIGTERM signal when this server must shutdown. */
  if (sigismember(&sigset, SIGTERM)) {
	exit(0);
  } 
  /* Ignore all other signals. */
}

/*===========================================================================*
 *				do_write				     *
 *===========================================================================*/
static void do_write(m_ptr, safe)
register kipc_msg_t *m_ptr;	/* pointer to the newly arrived message */
int safe;			/* use virtual addresses or grant id's? */
{
/* The printer is used by sending DEV_WRITE messages to it. Process one. */

    register int r = SUSPEND;
    int retries;
    unsigned long status;

    /* Reject command if last write is not yet finished, the count is not
     * positive, or the user address is bad.
     */
    if (writing)  			r = -EIO;
    else if (m_ptr->COUNT <= 0)  	r = -EINVAL;

    /* Reply to VFS_PROC_NR, no matter what happened, possible SUSPEND caller. */
    reply(KCNR_TASK_REPLY, m_ptr->m_source, m_ptr->IO_ENDPT, r);

    /* If no errors occurred, continue printing with SUSPENDED caller.
     * First wait until the printer is online to prevent stupid errors.
     */
    if (SUSPEND == r) { 	
	caller = m_ptr->m_source;
	proc_nr = m_ptr->IO_ENDPT;
	user_left = m_ptr->COUNT;
	orig_count = m_ptr->COUNT;
	user_vir_g = (vir_bytes) m_ptr->ADDRESS; /* Address or grant id. */
	user_vir_d = 0;				 /* Offset. */
	user_safe = safe;			 /* Address or grant? */
	writing = TRUE;
	grant_nr = safe ? (cp_grant_id_t) m_ptr->ADDRESS : GRANT_INVALID;

        retries = MAX_ONLINE_RETRIES + 1;  
        while (--retries > 0) {
            if(sys_inb(port_base + 1, &status) != 0) {
		printk("printer: sys_inb of %x failed\n", port_base+1);
		panic(__FILE__,"sys_inb failed", NO_NUM);
	    }
            if ((status & ON_LINE)) {		/* printer online! */
	        prepare_output();
	        do_printer_output();
	        return;
            }
            micro_delay(500000);		/* wait before retry */
        }
        /* If we reach this point, the printer was not online in time. */
        done_status = status;
        output_done();
    }
}

/*===========================================================================*
 *				output_done				     *
 *===========================================================================*/
static void output_done()
{
/* Previous chunk of printing is finished.  Continue if OK and more.
 * Otherwise, reply to caller (VFS_PROC_NR).
 */
    register int status;

    if (!writing) return;	  	/* probably leftover interrupt */
    if (done_status != 0) {      	/* printer error occurred */
        status = -EIO;
	if ((done_status & ON_LINE) == 0) { 
	    printk("Printer is not on line\n");
	} else if ((done_status & NO_PAPER)) { 
	    printk("Printer is out of paper\n");
	    status = -EAGAIN;	
	} else {
	    printk("Printer error, status is 0x%02X\n", done_status);
	}
	/* Some characters have been printed, tell how many. */
	if (status == -EAGAIN && user_left < orig_count) {
		status = orig_count - user_left;
	}
	oleft = 0;			/* cancel further output */
    } 
    else if (user_left != 0) {		/* not yet done, continue! */
	prepare_output();
	return;
    } 
    else {				/* done! report back to VFS_PROC_NR */
	status = orig_count;
    }
    revive_pending = TRUE;
    revive_status = status;
    kipc_module_call(KIPC_NOTIFY, 0, caller, 0);
}

/*===========================================================================*
 *				do_status				     *
 *===========================================================================*/
static void do_status(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to the newly arrived message */
{
  if (revive_pending) {
	m_ptr->m_type = DEV_REVIVE;		/* build message */
	m_ptr->REP_ENDPT = proc_nr;
	m_ptr->REP_STATUS = revive_status;
	m_ptr->REP_IO_GRANT = grant_nr;

	writing = FALSE;			/* unmark event */
	revive_pending = FALSE;			/* unmark event */
  } else {
	m_ptr->m_type = DEV_NO_STATUS;
  }
  kipc_module_call(KIPC_SEND, 0, m_ptr->m_source, m_ptr);			/* send the message */
}

/*===========================================================================*
 *				do_cancel				     *
 *===========================================================================*/
static void do_cancel(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to the newly arrived message */
{
/* Cancel a print request that has already started.  Usually this means that
 * the process doing the printing has been killed by a signal.  It is not
 * clear if there are race conditions.  Try not to cancel the wrong process,
 * but rely on VFS_PROC_NR to handle the -EINTR reply and de-suspension properly.
 */

  if (writing && m_ptr->IO_ENDPT == proc_nr) {
	oleft = 0;		/* cancel output by interrupt handler */
	writing = FALSE;
	revive_pending = FALSE;
  }
  reply(KCNR_TASK_REPLY, m_ptr->m_source, m_ptr->IO_ENDPT, -EINTR);
}

/*===========================================================================*
 *				reply					     *
 *===========================================================================*/
static void reply(code, replyee, process, status)
int code;			/* TASK_REPLY or REVIVE */
int replyee;			/* destination for message (normally VFS_PROC_NR) */
int process;			/* which user requested the printing */
int status;			/* number of  chars printed or error code */
{
/* Send a reply telling VFS_PROC_NR that printing has started or stopped. */

  kipc_msg_t pr_mess;

  pr_mess.m_type = code;		/* TASK_REPLY or REVIVE */
  pr_mess.REP_STATUS = status;		/* count or -EIO */
  pr_mess.REP_ENDPT = process;	/* which user does this pertain to */
  kipc_module_call(KIPC_SEND, 0, replyee, &pr_mess);		/* send the message */
}

/*===========================================================================*
 *				do_initialize				     *
 *===========================================================================*/
static void do_initialize()
{
/* Set global variables and initialize the printer. */
  static int initialized = FALSE;
  if (initialized) return;
  initialized = TRUE;
  
  /* Get the base port for first printer.  */
  if(sys_vircopy(ENDPT_SELF, BIOS_SEG, LPT1_IO_PORT_ADDR, 
  	ENDPT_SELF, D, (vir_bytes) &port_base, LPT1_IO_PORT_SIZE) != 0) {
	panic(__FILE__, "do_initialize: sys_vircopy failed", NO_NUM);
  }
  if(sys_outb(port_base + 2, INIT_PRINTER) != 0) {
	printk("printer: sys_outb of %x failed\n", port_base+2);
	panic(__FILE__, "do_initialize: sys_outb init failed", NO_NUM);
  }
  micro_delay(1000000/20);	/* easily satisfies Centronics minimum */
  if(sys_outb(port_base + 2, PR_SELECT) != 0) {
	printk("printer: sys_outb of %x failed\n", port_base+2);
	panic(__FILE__, "do_initialize: sys_outb select failed", NO_NUM);
  }
  irq_hook_id = 0;
  if(sys_irqsetpolicy(PRINTER_IRQ, 0, &irq_hook_id) != 0 ||
     sys_irqenable(&irq_hook_id) != 0) {
	panic(__FILE__, "do_initialize: irq enabling failed", NO_NUM);
  }
}

/*==========================================================================*
 *		    	      prepare_output				    *
 *==========================================================================*/
static void prepare_output()
{
/* Start next chunk of printer output. Fetch the data from user space. */
  int s;
  register int chunk;

  if ( (chunk = user_left) > sizeof obuf) chunk = sizeof obuf;
  if(user_safe) {
    s=sys_safecopyfrom(proc_nr, user_vir_g, user_vir_d,
    (vir_bytes) obuf, chunk, D);
  } else {
    s=sys_datacopy(proc_nr, user_vir_g, ENDPT_SELF, (vir_bytes) obuf, chunk);
  }

  if(s != 0) {
  	done_status = -EFAULT;
  	output_done();
  	return;
  }
  optr = obuf;
  oleft = chunk;
}

/*===========================================================================*
 *				do_printer_output				     *
 *===========================================================================*/
static void do_printer_output()
{
/* This function does the actual output to the printer. This is called on an
 * interrupt message sent from the generic interrupt handler that 'forwards'
 * interrupts to this driver. The generic handler did not reenable the printer
 * IRQ yet! 
 */

  unsigned long status;
  pvb_pair_t char_out[3];

  if (oleft == 0) {
	/* Nothing more to print.  Turn off printer interrupts in case they
	 * are level-sensitive as on the PS/2.  This should be safe even
	 * when the printer is busy with a previous character, because the
	 * interrupt status does not affect the printer.
	 */
	if(sys_outb(port_base + 2, PR_SELECT) != 0) {
		printk("printer: sys_outb of %x failed\n", port_base+2);
		panic(__FILE__,"sys_outb failed", NO_NUM);
	}
	if(sys_irqenable(&irq_hook_id) != 0) {
		panic(__FILE__,"sys_irqenable failed", NO_NUM);
	}
	return;
  }

  do {
	/* Loop to handle fast (buffered) printers.  It is important that
	 * processor interrupts are not disabled here, just printer interrupts.
	 */
	if(sys_inb(port_base + 1, &status) != 0) {
		printk("printer: sys_inb of %x failed\n", port_base+1);
		panic(__FILE__,"sys_inb failed", NO_NUM);
	}
	if ((status & STATUS_MASK) == BUSY_STATUS) {
		/* Still busy with last output.  This normally happens
		 * immediately after doing output to an unbuffered or slow
		 * printer.  It may happen after a call from prepare_output or
		 * pr_restart, since they are not synchronized with printer
		 * interrupts.  It may happen after a spurious interrupt.
		 */
		if(sys_irqenable(&irq_hook_id) != 0) {
			panic(__FILE__, "sys_irqenable failed\n", NO_NUM);
		}
		return;
	}
	if ((status & STATUS_MASK) == NORMAL_STATUS) {
		/* Everything is all right.  Output another character. */
		pv_set(char_out[0], port_base, *optr);	
		optr++;
		pv_set(char_out[1], port_base+2, ASSERT_STROBE);
		pv_set(char_out[2], port_base+2, NEGATE_STROBE);
		if(sys_voutb(char_out, 3) != 0) {
			/* request series of port outb */
			panic(__FILE__, "sys_voutb failed\n", NO_NUM);
		}

		user_vir_d++;
		user_left--;
	} else {
		/* Error.  This would be better ignored (treat as busy). */
		done_status = status;
		output_done();
		if(sys_irqenable(&irq_hook_id) != 0) {
			panic(__FILE__, "sys_irqenable failed\n", NO_NUM);
		}
		return;
	}
  }
  while (--oleft != 0);

  /* Finished printing chunk OK. */
  done_status = 0;
  output_done();
  if(sys_irqenable(&irq_hook_id) != 0) {
	panic(__FILE__, "sys_irqenable failed\n", NO_NUM);
  }
}

