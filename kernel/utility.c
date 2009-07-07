/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file contains a collection of miscellaneous procedures:
 *   minix_panic:    abort MINIX due to a fatal error
 *   kprintf:       (from lib/sysutil/kprintf.c)
 *   kputc:         buffered putc used by kernel kprintf
 */

#include <kernel/kernel.h>
#include <kernel/proc.h>

#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <nucleos/sysutil.h>
#include <nucleos/com.h>

/*===========================================================================*
 *			panic                                        *
 *===========================================================================*/
PUBLIC void panic(what, mess,nr)
char *what;
char *mess;
int nr;
{
/* This function is for when a library call wants to panic.
 * The library call calls printf() and tries to exit a process,
 * which isn't applicable in the kernel.
 */
	minix_panic(mess, nr);
}

/*===========================================================================*
 *			minix_panic                                        *
 *===========================================================================*/
PUBLIC void minix_panic(mess,nr)
char *mess;
int nr;
{
/* The system has run aground of a fatal kernel error. Terminate execution. */
  if (minix_panicing ++) return;		/* prevent recursive panics */

  if (mess != NULL) {
	kprintf("kernel panic: %s", mess);
	if(nr != NO_NUM)
		kprintf(" %d", nr);
	kprintf("\n");
  }

  kprintf("kernel stacktrace: ");
  util_stacktrace();

  /* Abort MINIX. */
  nucleos_shutdown(NULL);
}


/* Include system printf() implementation named kprintf() */

#define printf kprintf
#include "../lib/sysutil/kprintf.c"

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
PUBLIC void kputc(c)
int c;					/* character to append */
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output driver if an END_OF_KMESS is encountered. 
 */
  if (c != END_OF_KMESS) {
      if (do_serial_debug) {
	if(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      if (kmess.km_size < sizeof(kmess.km_buf))
          kmess.km_size += 1;		
      kmess.km_next = (kmess.km_next + 1) % KMESS_BUF_SIZE;
  } else {
      int p, outprocs[] = OUTPUT_PROCS_ARRAY;
      if(do_serial_debug) return;
      if(minix_panicing || do_serial_debug) return;
      for(p = 0; outprocs[p] != NONE; p++) {
	 if(isokprocn(outprocs[p]) && !isemptyn(outprocs[p])) {
           send_sig(outprocs[p], SIGKMESS);
	 }
      }
  }
}
