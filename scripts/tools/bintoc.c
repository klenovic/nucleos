/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/*
 * Convert a (binary) file to a series of comma separated hex values suitable
 * for initializing a character array in C.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *progname;
unsigned char buf[1024];

static void fatal(char *fmt, ...);
static void usage(void);

char *comment = "/*\n"
		" * Do not edit. This file was generated\n"
		" * by bintoc utility.\n"
		" */\n";

int main(int argc, char *argv[])
{
	int c, i, r, first;
	FILE *file_in, *file_out;
	char *in_name = 0;
	char *out_name = 0;
	int otype_s = 0;
	int otype_S = 0;

	(progname=strrchr(argv[0],'/')) ? progname++ : (progname=argv[0]);

	while (c= getopt(argc, argv, "hi:o:sS"), c != -1)
	{
		switch(c)
		{
		case 'h':
			usage();
			exit(0);

		case 'i':
			in_name = optarg;
			break;

		case 'o':
			out_name= optarg;
			break;

		case 's':
			/* generate code usable in .s file i.e. raw assembly */
			otype_s = 1;
			break;

		case 'S':
			/* generate code usable in .S file */
			otype_S = 1;
			break;

		default:
			fatal("getopt failed: '%c'\n", c);
		}
	}

	/* this is mandatory */
	if (!out_name) {
		usage();
		exit(1);
	}

	if (otype_s && otype_S)
		fatal("Specify either `-s' or `-S' not both!\n");

	if (out_name)
	{
		file_out= fopen(out_name, "w");
		if (file_out == NULL)
		{
			fatal("unable to create '%s': %s\n",
				out_name, strerror(errno));
			exit(1);
		}
	}
	else
		file_out= stdout;

	if (in_name)
	{
		file_in= fopen(in_name, "r");
		if (file_in == NULL)
		{
			fatal("unable to open '%s': %s",
				in_name, strerror(errno));
		}
	}
	else
	{
		in_name= "(stdin)";
		file_in= stdin;
	}


	first= 1;
	/* add comment */ 
	if (!otype_s)
		fprintf(file_out, comment);

	for (;;)
	{
		r= fread(buf, 1, sizeof(buf), file_in);
		if (r == 0)
			break;
		for (i= 0; i<r; i++)
		{
			if ((i % 8) == 0)
			{
				if (first)
				{
					fprintf(file_out, "\t");
					first= 0;
				} else {
					if (!otype_s && !otype_S)
						fprintf(file_out, ",\n\t");
					else
						fprintf(file_out, "\n\t");
				}

				if (otype_s)
					fprintf(file_out, ".byte\t");
			} else {
				fprintf(file_out, ", ");
			}

			fprintf(file_out, "0x%02x", buf[i]);
		}
	}

	if (ferror(file_in))
	{
		fatal("read error on %s: %s\n",
			in_name, strerror(errno));
	}
	fprintf(file_out, "\n");

	exit(0);
}

static void fatal(char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "%s: ", progname);

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\n");

	exit(1);
}

static void usage(void)
{
	fprintf(stderr, "Usage: bintoc [-s] [-i <in-file>] -o <out-file>\n");
	return;
}
