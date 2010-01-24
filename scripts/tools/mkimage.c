/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Create boot image from list of binaries.
 *
 * Original code from minix3's buildsystem branch. Adapted for nucleos
 * by L.Klenovic.
 */
#define _GNU_SOURCE	/* strnlen */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#define CONFIG_VERBOSE	0

#define BUF_SIZE	4096
#define HEADER_SIZE	0x20
#define HEADER_OFFSET	0x40
#define MAX_NAMELEN	32
#define SECTOR_SIZE	512

FILE * fout;

static int write_bytes(char *b, size_t bytes, off_t off)
{
	size_t written;

	fseek(fout, off, SEEK_SET);

	while (bytes && bytes > (written = fwrite(b, 1, bytes, fout))) {
		if (written == 0 && ferror(fout)) {
			printf("Write failed\n");
			return -1;
		}

		b += written;
		bytes -= written;
	}

	return 0;
}

static int write_name(char *str, off_t off)
{
	char *name;

	if (!str) 
		name = "noname";
	else
		name = basename(str);

	if (write_bytes(name, strnlen(name, MAX_NAMELEN), off)) {
		printf("Cant write name into image\n");
		return -1;
	}

	return 0;
}

int main(int argc, char ** argv)
{
	int i;
	char buff[BUF_SIZE];
	unsigned int osz = 0;

	if (argc < 3) {
		printf("usage <fout> <files in ...>\n");
		return -1;
	}

	fout = fopen(argv[1], "w");

	if (!fout) {
		printf("Cannot open output\n");
		return -1;
	}
#if CONFIG_VERBOSE
	printf("Building ELF boot image %s ...\n", argv[1]);
#endif
	for(i = 2; i < argc; i++)
	{
		FILE * fin;
		size_t bytes = 0;
#if CONFIG_VERBOSE
		printf("\t%s (0x%x)\n", argv[i], osz);
#endif
		fin = fopen(argv[i], "r");
		if (!fin) {
			printf("Cannot open file\n");
			return -1;
		}

		/* write name */
		write_name(argv[i], osz);

		/* read image header (32B) */
		if (fread(buff, 1, HEADER_SIZE, fin) != HEADER_SIZE) {
			printf("Cannot read header\n");
			return -1;
		}

		/* write image header */
		osz += HEADER_OFFSET;

		if (write_bytes(buff, HEADER_SIZE, osz)) {
			printf("Cannot write header\n");
			return -1;
		}

		/* pad to SECTOR_SIZE */
		osz += HEADER_SIZE;
		bytes = SECTOR_SIZE - (osz & (SECTOR_SIZE - 1));

		memset(buff, 0, bytes);

		if (write_bytes(buff, bytes, osz))
			return -1;

		osz += bytes;

		while ((bytes = fread(buff, 1, BUF_SIZE, fin))) {
			if (write_bytes(buff, bytes, osz))
				return -1;
			osz += bytes;
		}
	}
#if CONFIG_VERBOSE
	printf("Done\n");
#endif
	return 0;
}
