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
random.h

Public interface to the random number generator 
*/

/* Internal random sources */
#define RND_TIMING		0
#define RANDOM_SOURCES_INTERNAL	1
#define TOTAL_SOURCES	(RANDOM_SOURCES+RANDOM_SOURCES_INTERNAL)

void random_init(void);
int random_isseeded(void);
void random_update(int source, rand_t *buf, int count);
void random_getbytes(void *buf, size_t size);
void random_putbytes(void *buf, size_t size);
