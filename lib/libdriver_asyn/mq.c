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
inet/mq.c

Created:	Jan 3, 1992 by Philip Homburg

Copyright 1995 Philip Homburg
*/
#include <assert.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/mq.h>

#define MQ_SIZE		128

static mq_t mq_list[MQ_SIZE];
static mq_t *mq_freelist;

void mq_init()
{
	int i;

	mq_freelist= NULL;
	for (i= 0; i<MQ_SIZE; i++)
	{
		mq_list[i].mq_next= mq_freelist;
		mq_freelist= &mq_list[i];
		mq_list[i].mq_allocated= 0;
	}
}

mq_t *mq_get()
{
	mq_t *mq;

	mq= mq_freelist;
	assert(mq != NULL);

	mq_freelist= mq->mq_next;
	mq->mq_next= NULL;
	assert(mq->mq_allocated == 0);
	mq->mq_allocated= 1;
	return mq;
}

void mq_free(mq)
mq_t *mq;
{
	mq->mq_next= mq_freelist;
	mq_freelist= mq;
	assert(mq->mq_allocated == 1);
	mq->mq_allocated= 0;
}
