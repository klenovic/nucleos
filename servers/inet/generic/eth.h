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
eth.h

Copyright 1995 Philip Homburg
*/

#ifndef ETH_H
#define ETH_H

#define NWEO_DEFAULT    (NWEO_EN_LOC | NWEO_DI_BROAD | NWEO_DI_MULTI | \
	NWEO_DI_PROMISC | NWEO_REMANY | NWEO_RWDATALL)

#define eth_addrcmp(a,b) (memcmp((void*)&a, (void*)&b, \
	sizeof(a)))

/* Forward declatations */

struct acc;

/* prototypes */

void eth_prep(void);
void eth_init(void);
int eth_open(int port, int srfd, get_userdata_t get_userdata, put_userdata_t put_userdata,
	     put_pkt_t put_pkt, select_res_t sel_res);
int eth_ioctl(int fd, ioreq_t req);
int eth_read(int port, size_t count);
int eth_write(int port, size_t count);
int eth_cancel(int fd, int which_operation);
int eth_select(int fd, unsigned operations);
void eth_close(int fd);
int eth_send(int port, struct acc *data, size_t data_len);

#endif /* ETH_H */
