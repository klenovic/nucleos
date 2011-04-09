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
inet/proto.h

Created:	Jan 2, 1992 by Philip Homburg

Copyright 1995 Philip Homburg
*/

/* clock.c */

void clck_tick(kipc_msg_t *mess);

/* mnx_eth.c */

void eth_rec(kipc_msg_t *m);
void eth_check_drivers(kipc_msg_t *m);

/* sr.c */

struct mq;
void sr_rec(struct mq *m);
