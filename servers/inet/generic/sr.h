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
sr.h

Copyright 1995 Philip Homburg
*/

#ifndef SR_H
#define SR_H

#define MAX_IOCTL_S	512

#define SR_CANCEL_IOCTL	1
#define SR_CANCEL_READ	2
#define SR_CANCEL_WRITE	3

#define SR_SELECT_READ		0x01
#define SR_SELECT_WRITE		0x02
#define SR_SELECT_EXCEPTION	0x04
#define SR_SELECT_POLL		0x10

/* Forward struct declarations */

struct acc;

/* prototypes */

typedef int  (*sr_open_t)(int port, int srfd, get_userdata_t get_userdata,
			  put_userdata_t put_userdata,put_pkt_t put_pkt,
			  select_res_t select_res);
typedef void (*sr_close_t)(int fd);
typedef int (*sr_read_t)(int fd, size_t count);
typedef int (*sr_write_t)(int fd, size_t count);
typedef int (*sr_ioctl_t)(int fd, ioreq_t req);
typedef int (*sr_cancel_t)(int fd, int which_operation);
typedef int (*sr_select_t)(int fd, unsigned operations);

void sr_init(void);
void sr_add_minor(int minor, int port, sr_open_t openf, sr_close_t closef, sr_read_t sr_read,
		  sr_write_t sr_write, sr_ioctl_t ioctlf, sr_cancel_t cancelf, sr_select_t selectf);

#endif /* SR_H */

/* Track TCP connections back into sr (for lsof, identd, etc.) */
extern sr_cancel_t tcp_cancel_f;
