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
inet/proto.h

Created:	Jan 2, 1992 by Philip Homburg

Copyright 1995 Philip Homburg
*/

/* clock.c */

_PROTOTYPE( void clck_tick, (message *mess) );

/* mnx_eth.c */

_PROTOTYPE( void eth_rec, (message *m) );
_PROTOTYPE( void eth_check_drivers, (message *m) );

/* sr.c */

struct mq;
_PROTOTYPE( void sr_rec, (struct mq *m) );




/*
 * $PchId: proto.h,v 1.4 1995/11/21 06:36:37 philip Exp $
 */
