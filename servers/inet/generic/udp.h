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
udp.h

Copyright 1995 Philip Homburg
*/

#ifndef UDP_H
#define UDP_H

#define UDP_DEF_OPT		NWUO_NOFLAGS
#define UDP_MAX_DATAGRAM	40000 /* 8192 */
#define UDP_READ_EXP_TIME	(10L * HZ)
#define UDP_TOS			0
#define UDP_IP_FLAGS		0

#define UDP0	0

struct acc;

void udp_prep ARGS(( void ));
void udp_init ARGS(( void ));
int udp_open ARGS(( int port, int srfd,
	get_userdata_t get_userdata, put_userdata_t put_userdata, 
	put_pkt_t put_pkt, select_res_t select_res ));
int udp_ioctl ARGS(( int fd, ioreq_t req ));
int udp_read ARGS(( int fd, size_t count ));
int udp_write ARGS(( int fd, size_t count ));
void udp_close ARGS(( int fd ));
int udp_cancel ARGS(( int fd, int which_operation ));

#endif /* UDP_H */


/*
 * $PchId: udp.h,v 1.9 2005/06/28 14:12:05 philip Exp $
 */
