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
 * fillbuf.c - fill a buffer
 */
#include <nucleos/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "loc_incl.h"

ssize_t _read(ssize_t d, char *buf, size_t nbytes);

int
__fillbuf(register FILE *stream)
{
	static unsigned char ch[FOPEN_MAX];
	register int i;

	stream->_count = 0;
	if (fileno(stream) < 0) return EOF;
	if (io_testflag(stream, (_IOEOF | _IOERR ))) return EOF; 
	if (!io_testflag(stream, _IOREAD))
	     { stream->_flags |= _IOERR; return EOF; }
	if (io_testflag(stream, _IOWRITING))
	     { stream->_flags |= _IOERR; return EOF; }

	if (!io_testflag(stream, _IOREADING))
		stream->_flags |= _IOREADING;
	
	if (!io_testflag(stream, _IONBF) && !stream->_buf) {
		stream->_buf = (unsigned char *) malloc(BUFSIZ);
		if (!stream->_buf) {
			stream->_flags |= _IONBF;
		}
		else {
			stream->_flags |= _IOMYBUF;
			stream->_bufsiz = BUFSIZ;
		}
	}

	/* flush line-buffered output when filling an input buffer */
	for (i = 0; i < FOPEN_MAX; i++) {
		if (__iotab[i] && io_testflag(__iotab[i], _IOLBF))
			if (io_testflag(__iotab[i], _IOWRITING))
				(void) fflush(__iotab[i]);
	}

	if (!stream->_buf) {
		stream->_buf = &ch[fileno(stream)];
		stream->_bufsiz = 1;
	}
	stream->_ptr = stream->_buf;
	stream->_count = _read(stream->_fd, (char *)stream->_buf, stream->_bufsiz);

	if (stream->_count <= 0){
		if (stream->_count == 0) {
			stream->_flags |= _IOEOF;
		}
		else 
			stream->_flags |= _IOERR;

		return EOF;
	}
	stream->_count--;

	return *stream->_ptr++;
}
