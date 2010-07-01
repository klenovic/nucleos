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
inet/mq.h

Created:	Jan 3, 1992 by Philip Homburg

Copyright 1995 Philip Homburg
*/

#ifndef __NUCLEOS_MQ_H
#define __NUCLEOS_MQ_H

typedef struct mq
{
	kipc_msg_t mq_mess;
	struct mq *mq_next;
	int mq_allocated;
} mq_t;

mq_t *mq_get(void);
void mq_free(mq_t *mq);
void mq_init(void);

#endif /* __NUCLEOS_MQ_H */
