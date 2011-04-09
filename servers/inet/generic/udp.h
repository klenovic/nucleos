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

void udp_prep(void);
void udp_init(void);
int udp_open(int port, int srfd, get_userdata_t get_userdata, put_userdata_t put_userdata,
	     put_pkt_t put_pkt, select_res_t select_res);
int udp_ioctl(int fd, ioreq_t req);
int udp_read(int fd, size_t count);
int udp_write(int fd, size_t count);
void udp_close(int fd);
int udp_cancel(int fd, int which_operation);

#endif /* UDP_H */
