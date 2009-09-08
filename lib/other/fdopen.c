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
 * fdopen - convert a (UNIX) file descriptor into a FILE pointer
 */
#include	<stdlib.h>
#include	"../stdio/loc_incl.h"
#include	<stdio.h>
#include	<nucleos/stat.h>

FILE *
fdopen(fd, mode)
int fd;
const char *mode;
{
	register int i;
	struct stat st;
	FILE *stream;
	int flags = 0;

	if (fd < 0) return (FILE *)NULL;
	for (i = 0; __iotab[i] != 0 ; i++) 
		if (i >= FOPEN_MAX-1)
			return (FILE *)NULL;

	switch(*mode++) {
	case 'r':
		flags |= _IOREAD | _IOREADING;
		break;
	case 'a':
		flags |= _IOAPPEND;
	case 'w':
		flags |= _IOWRITE | _IOWRITING;
		break;
	default:
		return (FILE *)NULL;
	}
	while(*mode) {
		switch(*mode++) {
		case 'b':
			continue;
		case '+':
			flags |= _IOREAD | _IOWRITE;
			continue;
		/* The sequence may be followed by aditional characters */
		default:
			break;
		}
		break;
	}

	if ( fstat( fd, &st ) < 0 ) {
		return (FILE *)NULL;
	}
	
	if ( st.st_mode & S_IFIFO ) {
		flags |= _IOFIFO;
	}
	
	if ((stream = (FILE *) malloc(sizeof(FILE))) == NULL) {
		return (FILE *)NULL;
	}

	if ((flags & _IOREAD) && (flags & _IOWRITE))
		flags &= ~(_IOREADING | _IOWRITING);

	stream->_count = 0;
	stream->_fd = fd;
	stream->_flags = flags;
	stream->_buf = NULL;
	__iotab[i] = stream;
	return stream;
}
