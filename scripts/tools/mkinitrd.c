/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * Create initial ramdisk images that contain kernel, drivers, servers
 * needed in the early boot process, before the root file system
 * becomes available.
 * Note: For now it just adds aout header to filessystem image.
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "include/a.out.h"

void usage(char *progname)
{
	fprintf(stderr,
		"\tCreate initial ramdisk images that drivers and servers\n"
		"\tneeded in the early boot process, before the root file system\n"
		"\tbecomes available.\n"
		"\tNote: For now it just adds aout header to filessystem image.\n\n"
		"\tUsage: %s [-c cpu] [-h] [-l hdrlen] -i input -o output\n"
		"\t -c cpu: cpu type (386 or 8086)\n"
		"\t -f list of flags: list flags separated by `,' (see a.out.h for posible flags)\n"
		"\t -i input: input file (minix3 filesystem)\n"
		"\t -l hdrlen: length of header\n"
		"\t -o output: output initrd file\n"
		"\t -h : help\n", progname);

	return;
}

/* default header length */
#define PROGNAME	"mkinitrd"
#define HDRLEN		0x20
#define MAX_DIGITS	20
#define MAX_TOKENS	8
#define BUFSIZE		0x1000

/* Main program. */
int main(int argc, char **argv)
{
	struct MNX(exec) ahdr;   // minix aout header
	struct stat st_input;
	int fd_in;
	int fd_out;
	char* input = 0;
	char* output = 0;
	char strnum[MAX_DIGITS]; 
	char* strflags[MAX_TOKENS]; 
	unsigned int flags = A_IMG;
	unsigned char cpu = A_I80386;
	int hdrlen = HDRLEN;
	char buf[BUFSIZE];
	int opt;
	int i = 0;
	int n = 0;

	memset(strflags, 0, sizeof(strflags));
	memset(strnum, 0, sizeof(strnum));

	while ((opt = getopt(argc, argv,"c:f:hi:l:o:")) != -1) {
		switch (opt) {
		case 'c': /* cpu type */
			if (!strncmp(optarg, "i386", 4))
				cpu = A_I80386; 
			if (!strncmp(optarg, "i8086", 5))
				cpu = A_I8086;
			break;
		case 'f': /* orher flags */
			memset(strflags,0,MAX_TOKENS);
			flags = 0x00;

			/* get the first token */
			strflags[0] = strtok(optarg," ,");

			/* parse the rest of string */
			while ((++i <= MAX_TOKENS) &&
				((strflags[i] = strtok (0," ,")) != 0));

			n = i;
			unsigned int flag = 0x00;

			for (i=0; i<n; i++) {
				if (strlen(strflags[i]) > 2 && strflags[i][0] == '0' &&
					strflags[i][1] == 'x')
					sscanf(strflags[i],"%x",&flag);
				else
					flag = atoi(strflags[i]);

				flags |= flag;
			}
			break;

		case 'h': /* help */
			usage(PROGNAME);
			exit(0);

		case 'i': /* input */
			input = optarg;
			break;

		case 'l': /* header length (default 0x20) */
			memset(strnum,0,MAX_DIGITS);
			if (strlen(optarg) > 2 && optarg[0] == '0' && optarg[1] == 'x')
				sscanf(optarg,"%x",&hdrlen);
			else
				hdrlen = atoi(optarg);
			
			break;

		case 'o': /* output */
			output = optarg;
			break;

		default: /* '?' */
			usage(PROGNAME);
			exit(1);
		}
	}

	if (!input || !output) {
		usage(PROGNAME);
		exit(1);
	}

	if (stat(input, &st_input) != 0) {
		perror("stat");
		exit(1);
	}

	if ((fd_in = open(input, O_RDONLY)) < 0) {
		fprintf(stderr,"Couldn't open input file");
		exit(1);
	}

	if ((fd_out = open(output, O_CREAT|O_RDWR|O_TRUNC,
				S_IFREG|S_IRUSR|S_IWUSR|S_IWUSR)) < 0) {
		fprintf(stderr,"Couldn't open output file");
		exit(1);
	}

	/* Build a.out header and write to output */
	memset(&ahdr, 0, sizeof(ahdr));

	ahdr.a_magic[0] = A_MAGIC0;
	ahdr.a_magic[1] = A_MAGIC1;
	ahdr.a_flags    = flags | A_IMG; /* always A_IMG flag */
	ahdr.a_cpu      = cpu;
	ahdr.a_hdrlen   = hdrlen;
	ahdr.a_text     = 0;
	ahdr.a_data     = st_input.st_size;
	ahdr.a_bss      = 0;
	ahdr.a_total    = ahdr.a_data;

	write(fd_out, &ahdr, hdrlen);

	/* copy the input stuff into output */
	while ((n = read(fd_in, &buf, sizeof(buf))) > 0) {
		write(fd_out, &buf, n);
	}

	close(fd_in);
	close(fd_out);

	return 0;
}

