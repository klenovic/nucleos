/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * ftell.c - obtain the value of the file-position indicator of a stream
 */
#include	<stdio.h>

#if	(SEEK_CUR != 1) || (SEEK_SET != 0) || (SEEK_END != 2)
#error SEEK_* values are wrong
#endif

#include	"loc_incl.h"

#include	<nucleos/types.h>

off_t lseek(int fildes, off_t offset, int whence);

long ftell(FILE *stream)
{
	long result;
	int adjust = 0;

	if (io_testflag(stream,_IOREADING))
		adjust = -stream->_count;
	else if (io_testflag(stream,_IOWRITING)
		    && stream->_buf
		    && !io_testflag(stream,_IONBF))
		adjust = stream->_ptr - stream->_buf;
	else adjust = 0;

	result = lseek(fileno(stream), (off_t)0, SEEK_CUR);

	if ( result == -1 )
		return result;

	result += (long) adjust;
	return result;
}
