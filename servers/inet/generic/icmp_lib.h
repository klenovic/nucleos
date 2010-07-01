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
icmp_lib.h

Created Sept 30, 1991 by Philip Homburg

Copyright 1995 Philip Homburg
*/

#ifndef ICMP_LIB_H
#define ICMP_LIB_H

/* Prototypes */

void icmp_snd_parmproblem(acc_t *pack);
void icmp_snd_time_exceeded(int port_nr, acc_t *pack, int code);
void icmp_snd_unreachable(int port_nr, acc_t *pack, int code);
void icmp_snd_redirect(int port_nr, acc_t *pack, int code, ipaddr_t gw);
void icmp_snd_mtu(int port_nr, acc_t *pack, u16 mtu);

#endif /* ICMP_LIB_H */
