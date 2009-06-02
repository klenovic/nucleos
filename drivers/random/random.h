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
random.h

Public interface to the random number generator 
*/

/* Internal random sources */
#define RND_TIMING		0
#define RANDOM_SOURCES_INTERNAL	1
#define TOTAL_SOURCES	(RANDOM_SOURCES+RANDOM_SOURCES_INTERNAL)

_PROTOTYPE( void random_init, (void)					);
_PROTOTYPE( int random_isseeded, (void)					);
_PROTOTYPE( void random_update, (int source, rand_t *buf, int count)	);
_PROTOTYPE( void random_getbytes, (void *buf, size_t size)		);
_PROTOTYPE( void random_putbytes, (void *buf, size_t size)		);
