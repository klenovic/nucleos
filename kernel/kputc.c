/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <stdarg.h>
#include <nucleos/signal.h>
#include <nucleos/kernel.h>
#include <nucleos/types.h>
#include <nucleos/stddef.h>
#include <nucleos/com.h>
#include <nucleos/limits.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>

#define isdigit(c)	((unsigned) ((c) - '0') <  (unsigned) 10)
#define count_kputc(c) do { charcount++; kputc(c); } while(0)

void kputc(int c)
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

		if(!(kernel_in_panic || do_serial_debug)) {
			for(p = 0; outprocs[p] != ENDPT_NONE; p++) {
				if(isokprocn(outprocs[p]) && !isemptyn(outprocs[p])) {
					send_sig(outprocs[p], SIGKMESS);
				}
			}
		}
	}

	return;
}
