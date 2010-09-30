/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	printf() - system services printf()		Author: Kees J. Bot
 *								15 Jan 1994
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

int kprintf(const char *fmt, ...)
{
	int c, charcount = 0;
	enum { LEFT, RIGHT } adjust;
	enum { LONG, INT } intsize;
	int fill;
	int width, max, len, base;
	static char X2C_tab[]= "0123456789ABCDEF";
	static char x2c_tab[]= "0123456789abcdef";
	char *x2c;
	char *p;
	long i;
	unsigned long u;
	char temp[8 * sizeof(long) / 3 + 2];

	va_list argp;

	va_start(argp, fmt);

	while ((c= *fmt++) != 0) {
		if (c != '%') {
			/* Ordinary character. */
			count_kputc(c);
			continue;
		}

		/* Format specifier of the form:
		 *	%[adjust][fill][width][.max]keys
		 */
		c= *fmt++;

		adjust= RIGHT;
		if (c == '-') {
			adjust= LEFT;
			c= *fmt++;
		}

		fill= ' ';
		if (c == '0') {
			fill= '0';
			c= *fmt++;
		}

		width= 0;
		if (c == '*') {
			/* Width is specified as an argument, e.g. %*d. */
			width= va_arg(argp, int);
			c= *fmt++;
		} else
		if (isdigit(c)) {
			/* A number tells the width, e.g. %10d. */
			do {
				width= width * 10 + (c - '0');
			} while (isdigit(c= *fmt++));
		}

		max= INT_MAX;
		if (c == '.') {
			/* Max field length coming up. */
			if ((c= *fmt++) == '*') {
				max= va_arg(argp, int);
				c= *fmt++;
			} else
			if (isdigit(c)) {
				max= 0;
				do {
					max= max * 10 + (c - '0');
				} while (isdigit(c= *fmt++));
			}
		}

		/* Set a few flags to the default. */
		x2c= x2c_tab;
		i= 0;
		base= 10;
		intsize= INT;
		if (c == 'l' || c == 'L') {
			/* "Long" key, e.g. %ld. */
			intsize= LONG;
			c= *fmt++;
		}
		if (c == 0) break;

		switch (c) {
			/* Decimal. */
		case 'd':
			i= intsize == LONG ? va_arg(argp, long)
						: va_arg(argp, int);
			u= i < 0 ? -i : i;
			goto int2ascii;

			/* Octal. */
		case 'o':
			base= 010;
			goto getint;

			/* Pointer, interpret as %X or %lX. */
		case 'p':
			if (sizeof(char *) > sizeof(int)) intsize= LONG;

			/* Hexadecimal.  %X prints upper case A-F, not %lx. */
		case 'X':
			x2c= X2C_tab;
		case 'x':
			base= 0x10;
			goto getint;

			/* Unsigned decimal. */
		case 'u':
		getint:
			u= intsize == LONG ? va_arg(argp, unsigned long)
						: va_arg(argp, unsigned int);
		int2ascii:
			p= temp + sizeof(temp)-1;
			*p= 0;
			do {
				*--p= x2c[(ptrdiff_t) (u % base)];
			} while ((u /= base) > 0);
			goto string_length;

			/* A character. */
		case 'c':
			p= temp;
			*p= va_arg(argp, int);
			len= 1;
			goto string_print;

			/* Simply a percent. */
		case '%':
			p= temp;
			*p= '%';
			len= 1;
			goto string_print;

			/* A string.  The other cases will join in here. */
		case 's':
			p= va_arg(argp, char *);

		string_length:
			for (len= 0; p[len] != 0 && len < max; len++) {}

		string_print:
			width -= len;
			if (i < 0) width--;
			if (fill == '0' && i < 0) count_kputc('-');
			if (adjust == RIGHT) {
				while (width > 0) { count_kputc(fill); width--; }
			}
			if (fill == ' ' && i < 0) count_kputc('-');
			while (len > 0) { count_kputc((unsigned char) *p++); len--; }
			while (width > 0) { count_kputc(fill); width--; }
			break;

			/* Unrecognized format key, echo it back. */
		default:
			count_kputc('%');
			count_kputc(c);
		}
	}

	/* Mark the end with a null (should be something else, like -1). */
	kputc(0);
	va_end(argp);

	return charcount;
}
