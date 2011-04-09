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
arp.h

Copyright 1995 Philip Homburg
*/

#ifndef ARP_H
#define ARP_H

#define ARP_ETHERNET	1

#define ARP_REQUEST	1
#define ARP_REPLY	2

/* Prototypes */
typedef void (*arp_func_t)(int fd, ipaddr_t ipaddr, ether_addr_t *ethaddr);
void arp_prep(void);
void arp_init(void);
void arp_set_ipaddr(int eth_port, ipaddr_t ipaddr);
int arp_set_cb(int eth_port, int ip_port, arp_func_t arp_func);
int arp_ip_eth(int eth_port, ipaddr_t ipaddr, ether_addr_t *ethaddr);
int arp_ioctl(int eth_port, int fd, ioreq_t req, get_userdata_t get_userdata,
	      put_userdata_t put_userdata);

#endif /* ARP_H */
